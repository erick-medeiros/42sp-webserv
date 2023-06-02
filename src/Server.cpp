#include "Server.hpp"

Server::Server(std::string config_file)
{
	// TODO: Ignoring config_file for now
	(void) config_file;

	listenToPort(8080);
}

Server::~Server()
{
	close(serverSocket);
}

// Method to listen to a port called by the constructor
int Server::listenToPort(int port)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		logError("--- Error: socket");
		exit(1);
	}
	logInfo("Server socket created", serverSocket);
	// --- Associate socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(serverSocket, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		logError("--- Error: binding");
		close(serverSocket);
		exit(1);
	}
	// --- Set socket to listen for connections ---
	if (listen(serverSocket, 5) < 0)
	{
		logError("--- Error: listen");
		exit(1);
	}
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

int Server::waitForClient(void)
{
	sockaddr_in clientAddr;
	socklen_t   clilen = sizeof(clientAddr);

	logNotice("Waiting new connection in port", getPort());
	int clientSocket =
	    accept(serverSocket, (struct sockaddr *) &clientAddr, &clilen);

	if (clientSocket < 0)
	{
		logError("--- Error: while accepting a new connection");
		return -1;
	}
	logSuccess("+++ Client connected in socket", clientSocket);
	return clientSocket;
}

std::string Server::getRawRequest(int clientSocket)
{
	// --- Request ---
	char buff[2048];
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
	while (true)
	{
		// --- Accept connection ---
		int clientSocket = waitForClient();
		if (clientSocket == -1)
			continue;

		// --- Request ---
		try
		{
			std::string rawRequest = getRawRequest(clientSocket);
			Request     request(rawRequest);
			request.displayInfo();
		}
		catch (std::exception const &e)
		{
			logError(e.what());
			close(clientSocket); // TODO: handle exception properly...
			continue;
		}

		// --- Response ---
		Response response;
		response.loadFile("src/index.html"); // TODO: Load the path asked by request
		response.sendTo(clientSocket);

		// --- Close connection ---
		logWarning("--- Client disconnected from socket", clientSocket);
		close(clientSocket);
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
