#include "Server.hpp"
#include "HttpStatus.hpp"

Server::Server(void) : _serverSocket(0) {}

void Server::init(Config const &conf)
{
	_config = conf;

	log.info("Server init in port " + utils::to_string(_config.getPort()));

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
		log.error("socket", strerror(errno));
		exit(1);
	}
	log.info("Server socket created " + utils::to_string(_serverSocket));
	// --- Configure socket options ---
	int yes = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		log.error("setsockopt", strerror(errno));
		exit(1);
	}
	// --- Bind socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(_serverSocket, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		log.error("bind port " + utils::to_string(port), strerror(errno));
		close(_serverSocket);
		exit(1);
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(_serverSocket))
	{
		exit(1);
	}
	// --- Set socket to listen for connections ---
	if (listen(_serverSocket, 5) < 0)
	{
		log.error("listen", strerror(errno));
		exit(1);
	}
	log.info("Server listening on port " + utils::to_string(port));
	return _serverSocket;
}

int Server::getPort()
{
	sockaddr_in address;
	socklen_t   len = sizeof(address);
	if (getsockname(_serverSocket, (struct sockaddr *) &address, &len) < 0)
	{
		log.error("getsockname", strerror(errno));
		exit(1);
	}
	return ntohs(address.sin_port);
}

std::string Server::getRequestData(int clientSocket)
{
	int  buffSize = 1024;
	char buff[buffSize];

	int bytesRead = recv(clientSocket, buff, sizeof(buff) - 1, 0);

	if (bytesRead == -1)
	{
		log.warning("Error: while receiving data");
		return "";
	}
	if (bytesRead == 0)
	{
		log.warning("Error: client has closed its connection");
		close(clientSocket);
		return "";
	}
	buff[bytesRead] = 0;
	log.info("Request size " + utils::to_string(bytesRead));
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

static int loadIndex(const Config &config, Response &response, std::string &fullPath)
{
	std::istringstream       iss(config.getIndex());
	std::vector<std::string> tokens;

	std::string token;
	while (iss >> token)
	{
		tokens.push_back(token);
	}

	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end();
	     it++)
	{
		std::string path = fullPath + *it;
		if (utils::isFile(path))
		{
			response.loadFile(path);
			break;
		}
	}
	return 0;
}

int Server::handleRequest(Connection &connection)
{
	Request    &request = connection.request;
	Config     &config = connection.config;
	Response   &response = connection.response;
	std::string serverRoot = config.getMainRoot();
	std::string requestPath = request.getResourcePath();
	std::string fullPath = serverRoot + requestPath;

	std::vector<location_t> locations = connection.config.getLocations(requestPath);
	if (!locations.empty())
	{
		for (size_t i = 0; i < locations.size(); ++i)
		{
			std::string requestMethod = request.getMethod();
			if (!utils::contains(locations[i].http_methods, requestMethod))
			{
				log.error("invalid http method");
				response.setStatus(HttpStatus::NOT_IMPLEMENTED);
				return 0;
			}
		}
	}

	// Prepare response with custom error pages
	std::map<int, std::string>           errorPages = _config.getErrorPages();
	std::map<int, std::string>::iterator it;
	for (it = errorPages.begin(); it != errorPages.end(); it++)
	{
		response.setCustomErrorPage(it->first, it->second);
	}

	// Request asked for a path that does not exist
	if (!utils::pathExists(fullPath))
	{
		response.setStatus(HttpStatus::NOT_FOUND);
		return 0;
	}

	// If there is cgi enabled for this location
	if (config.hasCGI(requestPath))
	{
		CGIRequest  cgi(fullPath, connection);
		std::string result = cgi.exec();
		response.setBody(result);
		return 0;
	}

	// Request is a directory and autoindex is enabled

	if (utils::isDir(fullPath))
	{
		loadIndex(config, response, fullPath);
		if (requestPath != "/")
		{
			if (_config.directoryListingEnabled(requestPath))
			{
				response.listDir(requestPath);
			}
			else
			{
				response.setStatus(HttpStatus::NOT_FOUND);
			}
		}
	}

	// Request is a regular file
	else if (utils::isFile(fullPath))
	{
		response.loadFile(fullPath);
	}

	// Rquest body is too large
	if (request.getBody().size() > _config.getClientBodySize())
	{
		response.setStatus(HttpStatus::PAYLOAD_TOO_LARGE);
	}

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
