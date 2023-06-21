#include "Server.hpp"

Server::Server(void) : _serverSocket(0) {}

void Server::init(Config const &conf)
{
	_config = conf;
	logSuccess("initializing new web server");

	listenToPort(_config.getPort());
}

Server::~Server()
{
	close(_serverSocket);
}

int Server::listenToPort(int port)
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		logError("--- Error: socket");
		exit(1);
	}
	logInfo("Server socket created", _serverSocket);
	// --- Configure socket options ---
	int yes = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		logError("--- Error: setsockopt");
		exit(1);
	}
	// --- Bind socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(_serverSocket, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		logError("--- Error: binding");
		close(_serverSocket);
		exit(1);
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(_serverSocket))
	{
		logError("--- Error: Set non-blocking");
		exit(1);
	}
	// --- Set socket to listen for connections ---
	if (listen(_serverSocket, 5) < 0)
	{
		logError("--- Error: listen");
		exit(1);
	}
	logInfo("Server listening on port", port);
	return _serverSocket;
}

int Server::getPort()
{
	sockaddr_in address;
	socklen_t   len = sizeof(address);
	if (getsockname(_serverSocket, (struct sockaddr *) &address, &len) < 0)
	{
		logError("--- Error: getsockname");
		exit(1);
	}
	return ntohs(address.sin_port);
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
	return _serverSocket;
}

Config &Server::getConfig(void)
{
	return _config;
}

Response Server::handleRequest(const Request &request, Connection &connection)
{
	Response    response;
	std::string serverRoot = _config.getRoot();
	std::string requestMethod = request.getMethod();
	std::string requestPath = request.getResourcePath();
	std::string filePath = serverRoot + requestPath;

	// Overwrite default error pages with config error pages
	if (_config.hasErrorPages(requestPath))
	{
		std::map<int, std::string> errorPages = _config.getErrorPages(requestPath);
		std::map<int, std::string>::iterator it;
		for (it = errorPages.begin(); it != errorPages.end(); it++)
		{
			response.setErrorPage(it->first, it->second); // EX: 404, custom404.html
		}
	}

	// -- SITUATIONS WITH EARLY RETURN --
	
	// Config defined a specific return code
	if (_config.hasReturn(requestPath))
	{
		response.setStatus(_config.getReturnCode(requestPath));
		response.setHeader("Location", _config.getReturnLocation(requestPath));
		return response;
	}

	// Request host is not in the config server names
	if (!_config.hasServerName(request.getHost()))
	{
		response.setStatus(HttpStatus::BAD_REQUEST);
		return response;
	}

	// Requested method is not accepted for that route
	std::vector<std::string> methods = _config.getMethods(requestPath);
	if (std::find(methods.begin(), methods.end(), requestMethod) == methods.end())
	{
		response.setStatus(HttpStatus::METHOD_NOT_ALLOWED);
		return response;
	}

	// Request asked for a file that does not exist
	if (!utils::fileExists(filePath))
	{
		response.setStatus(HttpStatus::NOT_FOUND);
		return response;
	}

	// --- SITUATIONS WITH LATE RETURN ---



	// TODO: Tem como simplificar um pouco a chamad do CGI?
	// Acho que o primeiro if já podia checar se é valido aí não precisaria o if/else
	// Pensei algo nesse formato: 
	// if (_config.isCGI(requestPath) && CGIRequest::isValid(requestPath))
	// {
	// 	std::string scriptOutput = CGIRequest::executeScript(requestPath);
	// 	response.setBody(scriptOutput);
	// }

	// Request is a CGI script
	if (CGIRequest::isValidScriptLocation(requestPath, connection))
	{
		CGIRequest cgi(requestPath, connection);

		// TODO: O if em cima já podeia checar isso, aí não precisa desse if/else
		if (cgi.isValid()) 
		{
			request.setCgiAs(true);
			int status;
			int pid = fork();
			if (pid == 0)
			{
				cgi.exec(*request, connection.config.getPort());
			}
			waitpid(pid, &status, 0);
		}
		else
		{
			request.setErrorCode(HttpStatus::NOT_FOUND);
			request.setCgiAs(false);
		}
		return response;
	}

	// Request is a directory - try to load an index file
	if (utils::isDir(filePath))
	{
		std::vector<std::string> indexFiles = _config.getIndexFiles();

		std::vector<std::string>::iterator it;
		for (it = indexFiles.begin(); it != indexFiles.end(); it++)
		{
			if (utils::fileExists(filePath + "/" + *it))
			{
				response.loadFile(filePath + "/" + *it);
				break;
			}
		}
	}

	// Request is a directory and autoindex is enabled
	if (_config.hasAutoIndex(requestPath) && utils::isDir(filePath))
	{
		response.listDir(filePath);
	}

	// Request is a regular file
	if (utils::isFile(filePath))
	{
		response.loadFile(filePath);
	}

	// Rquest body is too large, return 413
	if (request.getBody().size() > _config.getClientBodySize())
	{
		response.setStatus(HttpStatus::PAYLOAD_TOO_LARGE);
	}

	return response;
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
