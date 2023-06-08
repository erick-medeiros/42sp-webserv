#include "Server.hpp"

Server::Server(void) {}

void Server::init(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	logSuccess("initializing new web server");

	initSignal(this);

	listenToPort(8080);
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

std::string Server::readFromSocket(int clientSocket)
{
	char buff[2048];
	int  bytesRead = recv(clientSocket, buff, sizeof(buff), 0);

	if (bytesRead == -1)
	{
		logError("--- Error: while receiving data");
		return "";
	}
	if (bytesRead == 0)
	{
		logError("--- Error: client has closed its connection");
		close(clientSocket);
		return "";
	}
	buff[bytesRead] = 0;
	logInfo("Request size", bytesRead);
	return std::string(buff);
}

int Server::disconnectClient(Request *request)
{
	if (!request)
		return 0;
	int fd = request->getFd();
	logInfo("--- Client disconnected from socket", fd);
	monitoredSockets.remove(fd);
	delete request;
	return close(fd);
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];

	// --- Add server socket to waiting list, so it is managed by epoll ---
	monitoredSockets.add(serverSocket, EPOLLIN | EPOLLOUT);

	while (true)
	{
		int numEvents = monitoredSockets.wait(events, MAX_EVENTS, BLOCK_IND);

		for (int i = 0; i < numEvents; ++i)
		{
			Request *request = (Request *) events[i].data.ptr;
			int      clientSocket = request->getFd();

			// New connection on server
			if (clientSocket == serverSocket)
			{
				int newClient = acceptNewClient();
				setNonBlocking(newClient);
				monitoredSockets.add(newClient, EPOLLIN);
				continue;
			}

			if (events[i].events & EPOLLERR)
			{
				logError("Epoll error on socket", clientSocket);
				disconnectClient(request);
				continue;
			}

			if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
			{
				logError("Client disconnected from socket", clientSocket);
				disconnectClient(request);
				continue;
			}

			// Request
			if (events[i].events & EPOLLIN)
			{
				try
				{
					std::string rawInput = readFromSocket(clientSocket);
					request->parse(rawInput);
					std::cout << *request << std::endl;
				}
				catch (std::exception const &e)
				{
					logError(e.what());
					disconnectClient(request);
					// TODO: handle exception properly...
					continue;
				}
				// TODO: Esperar request estar "pronta" pra enviar resposta
				monitoredSockets.modify(events[i], EPOLLOUT);
				continue;
			}

			// Response
			if (events[i].events & EPOLLOUT)
			{
				Response response(*request);
				response.sendTo(clientSocket);
				disconnectClient(request);
			}
		}
	}

	monitoredSockets.remove(serverSocket);
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
