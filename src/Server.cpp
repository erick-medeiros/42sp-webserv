#include "Server.hpp"

Server::Server(void) : _requestSocket(0), serverSocket(0) {}

void Server::init(Config const &conf)
{
	_config = conf;

	logSuccess("initializing new web server");

	listenToPort(_config.getPort());

	_requestSocket = new Request(serverSocket);
}

Server::~Server()
{
	delete _requestSocket;
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
		logError("--- Error: Set non-blocking");
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
		logError("--- Error: accept", strerror(errno));
		exit(1);
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(clientSocket))
	{
		logError("--- Error: Set non-blocking");
		exit(1);
	}
	logSuccess("+++ New connection accepted on socket", clientSocket);
	return clientSocket;
}

std::string Server::getRequestData(Request *request)
{
	int clientSocket = request->getFd();

	int  buffSize = 1024;
	char buff[buffSize];

	int bytesRead = recv(clientSocket, buff, sizeof(buff), 0);

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

int Server::getServerSocket()
{
	return serverSocket;
}

Config &Server::getConfig(void)
{
	return _config;
}

Request *Server::getRequestSocket(void)
{
	return _requestSocket;
}

int Server::requestClient(Request *request)
{
	std::string rawRequest = getRequestData(request);
	request->parse(rawRequest);
	std::cout << *request << std::endl;

	// Handle CGI
	std::string resource = request->getResourcePath();
	resource = trim(resource);

	if (resource.find_last_of(".php") != std::string::npos)
	{
		CGIRequest cgi(resource);
		if (cgi.isValid())
		{
			request->setCgiAs(true);
			int status;
			int pid = fork();
			if (pid == 0)
			{
				cgi.exec(*request);
			}
			waitpid(pid, &status, 0);
		}
	}
	else
	{
		request->setCgiAs(false);
	}

	return 0;
}

#ifndef FEATURE_FLAG_COOKIE
#define FEATURE_FLAG_COOKIE 0
#endif

int Server::responseClient(Request *request, Config &config, Cookie &cookies)
{
	Response response(*request);

	if (FEATURE_FLAG_COOKIE) // test
	{
		string username = Cookie::getUsername(*request);
		if (username == "")
		{
			string value = Cookie::getValueCookie(*request, "session");
			if (cookies.get(value) != "")
			{
				response.setStatus(200);
				response.setBody("username " + cookies.get(value));
			}
		}
		else
		{
			string session = cookies.generateSession();
			cookies.set(session, username);
			response.setHeader("Set-Cookie", "session=" + session + ";path=/");
		}
	}

	// TODO: add other errors
	if (response.getStatusCode() >= 400 || response.getStatusCode() <= 599)
	{
		// Config &config
		string error_page = config.getErrorPage(response.getStatusCode());
		if (error_page.size() > 0)
			response.loadFile(error_page);
	}
	response.sendHttpResponse();

	return 0;
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
