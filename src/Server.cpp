#include "Server.hpp"

Server::Server(void) {}

void Server::init(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	logSuccess("initializing new web server");

	initSignal(this);

	listenToPort(8080);

	// --- Add server socket to waiting list, so it is managed by epoll ---
	waiting_list.add(serverSocket, EPOLLIN | EPOLLOUT);
}

Server::~Server()
{
	close(serverSocket);
}

int Server::listenToPort(int port)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		logError("--- Error: socket");
		exit(1);
	}
	logInfo("Server socket created", serverSocket);
	// --- Configure socket options ---
	int yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		logError("--- Error: setsockopt");
		exit(1);
	}
	// --- Bind socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(serverSocket, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		logError("--- Error: binding");
		close(serverSocket);
		exit(1);
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(serverSocket))
	{
		logError("--- Error: fcntl");
		exit(1);
	}
	// --- Set socket to listen for connections ---
	if (listen(serverSocket, 5) < 0)
	{
		logError("--- Error: listen");
		exit(1);
	}
	logInfo("Server listening on port", port);
	return serverSocket;
}

int Server::getPort()
{
	sockaddr_in address;
	socklen_t   len = sizeof(address);
	if (getsockname(serverSocket, (struct sockaddr *) &address, &len) < 0)
	{
		logError("--- Error: getsockname");
		exit(1);
	}
	return ntohs(address.sin_port);
}

int Server::acceptNewClient(void)
{
	sockaddr_in clientAddr;
	socklen_t   clilen = sizeof(clientAddr);

	int clientSocket =
	    accept(serverSocket, (struct sockaddr *) &clientAddr, &clilen);
	if (clientSocket == -1)
	{
		logError("--- Error: accept");
		exit(1);
	}
	logSuccess("+++ New connection accepted on socket", clientSocket);
	return clientSocket;
}

std::string Server::getRawRequest(int clientSocket)
{
	char buff[1024];
	int  bytesWritten = recv(clientSocket, buff, sizeof(buff), 0);

	if (bytesWritten == -1)
	{
		logError("--- Error: while receiving data");
		return "";
	}
	buff[bytesWritten] = 0;

	if (bytesWritten == 0)
	{
		logError("--- Error: client has closed its connection");
		close(clientSocket);
		return "";
	}
	logInfo("Request size", bytesWritten);
	return std::string(buff);
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];

	while (true)
	{
		int events_count = waiting_list.wait(events, MAX_EVENTS, -1);

		for (int i = 0; i < events_count; ++i)
		{
			// New connection on server
			if (events[i].data.fd == serverSocket)
			{
				int newClient = acceptNewClient();
				if (!setNonBlocking(newClient))
					logError("--- Error: set nonblocking: new client");
				waiting_list.add(newClient, EPOLLIN);
			}
			// Data coming in ( Request )
			else if (events[i].events & EPOLLIN)
			{
				int clientSocket = events[i].data.fd;
				try
				{
					std::string rawRequest = getRawRequest(clientSocket);
					Request     request(rawRequest);
					std::cout << request << std::endl;
				}
				catch (std::exception const &e)
				{
					logError(e.what());
					logInfo("--- Client disconnected from socket", clientSocket);
					close(clientSocket); // TODO: handle exception properly...
					continue;
				}
				waiting_list.modify(clientSocket, EPOLLOUT);
			}
			// Response
			else if (events[i].events & EPOLLOUT)
			{
				int      clientSocket = events[i].data.fd;
				Response response;
				response.loadFile("src/index.html");
				response.sendTo(clientSocket);
				logInfo("--- Client disconnected from socket", clientSocket);
				close(clientSocket);
			}
			// Client disconnected
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
			{
				int clientSocket = events[i].data.fd;
				logInfo("--- Client disconnected from socket", clientSocket);
				close(clientSocket);
				continue;
			}
			if (events[i].events & EPOLLERR)
			{
				logError("epoll error");
			}
		}
	}
}

// --- Helper functions ---
sockaddr_in Server::createServerAddress(int port)
{
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	return address;
}
