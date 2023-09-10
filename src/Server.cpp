#include "Server.hpp"

static bool setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		logger.error("fcntl", strerror(errno));
		return false;
	}
	return true;
}

Server::Server(void) : _serverSocket(0) {}

void Server::init(Config const &conf)
{
	_config = conf;

	_serverSocket = listenToPort(_config.getPort());
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
		logger.error("socket", strerror(errno));
		return -1;
	}
	logger.info("Server socket created " + utils::to_string(_serverSocket));
	// --- Configure socket options ---
	int yes = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		logger.error("setsockopt", strerror(errno));
		return -1;
	}
	// --- Bind socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(_serverSocket, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		logger.error("bind port " + utils::to_string(port), strerror(errno));
		close(_serverSocket);
		return -1;
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(_serverSocket))
	{
		return -1;
	}
	// --- Set socket to listen for connections ---
	if (listen(_serverSocket, 5) < 0)
	{
		logger.error("listen", strerror(errno));
		return -1;
	}
	logger.info("Server listening on port " + utils::to_string(port));
	return _serverSocket;
}

int Server::getPort()
{
	sockaddr_in address;
	socklen_t   len = sizeof(address);
	if (getsockname(_serverSocket, (struct sockaddr *) &address, &len) < 0)
	{
		logger.error("getsockname", strerror(errno));
		return -1;
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
		logger.warning("Error: while receiving data");
		close(clientSocket);
		return "";
	}
	if (bytesRead == 0)
	{
		logger.warning("Error: client has closed its connection");
		close(clientSocket);
		return "";
	}
	buff[bytesRead] = 0;
	return std::string(buff, bytesRead);
}

int Server::getServerSocket()
{
	return _serverSocket;
}

Config &Server::getConfig(void)
{
	return _config;
}

static Cookie cookies;

void loadCustomErrorPages(Config &config, Response &response)
{
	std::map<int, std::string>           errorPages = config.getErrorPages();
	std::map<int, std::string>::iterator it;
	for (it = errorPages.begin(); it != errorPages.end(); it++)
	{
		response.setCustomErrorPage(it->first, it->second);
	}
}

bool requestHasError(Request &request)
{
	return (request.getErrorCode() != 0);
}

bool requestHasInvalidMethod(Request &request, Config &config)
{
	std::vector<location_t> locations =
	    config.getLocations(request.getResourcePath());
	if (!locations.empty())
	{
		for (size_t i = 0; i < locations.size(); ++i)
		{
			std::string requestMethod = request.getMethod();
			if (!utils::contains(locations[i].http_methods, requestMethod))
			{
				return true;
			}
		}
	}
	return false;
}

bool locationHasRedirection(std::vector<location_t> &locations)
{
	if (!locations.empty())
	{
		for (size_t i = 0; i < locations.size(); ++i)
		{
			if (locations[i].http_redirection.first != 0)
			{
				return true;
			}
		}
	}
	return false;
}

std::pair<int, std::string> getRedirection(std::vector<location_t> &locations)
{
	for (size_t i = 0; i < locations.size(); ++i)
	{
		if (locations[i].http_redirection.first != 0)
		{
			return locations[i].http_redirection;
		}
	}
	return std::pair<int, std::string>(0, "");
}

void Server::handleRequest(Connection &connection)
{
	Request    &request = connection.request;
	Config     &config = connection.config;
	Response   &response = connection.response;
	std::string serverRoot = config.getMainRoot();
	std::string requestPath = request.getResourcePath();
	std::string fullPath = serverRoot + requestPath;

	loadCustomErrorPages(config, response);

	if (requestHasError(request))
	{
		response.setStatus(request.getErrorCode());
		return;
	}

	if (requestHasInvalidMethod(request, config))
	{
		response.setStatus(HttpStatus::METHOD_NOT_ALLOWED);
		return;
	}

	std::vector<location_t> locations =
	    config.getLocations(request.getResourcePath());
	if (!locations.empty())
	{
		if (handleCookies(connection, locations))
			return;
	}

	if (locationHasRedirection(locations))
	{
		std::pair<int, std::string> redirection = getRedirection(locations);
		response.setStatus(redirection.first);
		response.setHeader("Location", redirection.second);
		return;
	}

	if (request.isMultipart())
	{
		handleMultipart(connection);
		response.setStatus(HttpStatus::SEE_OTHER);
		response.setHeader("Location", request.getResourcePath());
		return;
	}

	if (!utils::pathExists(fullPath))
	{
		response.setStatus(HttpStatus::NOT_FOUND);
		return;
	}

	// If there is cgi enabled for this location
	if (config.hasCGI(requestPath))
	{
		CGIRequest  cgi(fullPath, connection);
		std::string result = cgi.exec();
		response.setBody(result);
		return;
	}

	// Rquest body is too large
	if (request.getBody().size() > _config.getClientBodySize())
	{
		response.setStatus(HttpStatus::PAYLOAD_TOO_LARGE);
		return;
	}

	// Request is a directory and autoindex is enabled
	if (utils::isDir(fullPath))
	{
		bool notFound = true;
		for (std::set<std::string>::const_iterator index = config.getIndex().begin();
		     index != config.getIndex().end(); index++)
		{
			std::string path = utils::end_with(fullPath, "/") ?
			                       fullPath + *index :
			                       fullPath + "/" + *index;
			if (utils::isFile(path))
			{
				response.loadFile(path);
				notFound = false;
				break;
			}
		}
		if (notFound && requestPath != "/")
		{
			if (_config.directoryListingEnabled(requestPath))
			{
				response.listDir(serverRoot, requestPath);
			}
			else
			{
				response.setStatus(HttpStatus::NOT_FOUND);
			}
		}
	}

	// Request is a regular file
	if (utils::isFile(fullPath))
	{
		if (request.getMethod() == "DELETE")
		{
			if (remove(fullPath.c_str()) == 0)
				response.setStatus(HttpStatus::OK);
			else
				response.setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
		}
		else
			response.loadFile(fullPath);
		return;
	}

	return;
}

int Server::handleCookies(Connection &connection, std::vector<location_t> &locations)
{
	Request  &request = connection.request;
	Response &response = connection.response;

	for (size_t i = 0; i < locations.size(); ++i)
	{
		if (locations[i].required_cookie.size() > 0)
		{
			std::set<std::string>::const_iterator it =
			    locations[i].required_cookie.begin();
			while (it != locations[i].required_cookie.end())
			{
				const std::string &nameCookie = *it;
				std::string        valueCookie = request.getValueCookie(nameCookie);
				t_cookie           cookie = cookies.get(nameCookie, valueCookie);
				if (valueCookie == "" || cookie.value == "")
				{
					response.setStatus(HttpStatus::FORBIDDEN);
					return 1;
				}
				else
				{
					response.setHeader("Cookie-" + nameCookie, cookie.value);
				}
				it++;
			}
		}
		if (locations[i].set_cookie.size() > 0)
		{
			for (std::vector<t_cookie>::const_iterator it =
			         locations[i].set_cookie.begin();
			     it != locations[i].set_cookie.end(); it++)
			{
				t_cookie cookie = *it;

				if (it->sessionValue)
				{
					std::string SetCookieValue = request.getParam(cookie.name);

					logger.info("SetCookieValue: " + SetCookieValue);

					if (SetCookieValue == "")
					{
						continue;
					}

					std::string session = cookies.generateSession();
					cookie.value = SetCookieValue;
					cookies.set(session, cookie);
					logger.debug("create cookie: " + cookie.name + " " + session +
					             " value " + cookie.value);
					cookie.value = session;
				}

				std::string setCookieHeader = cookie.name + "=" + cookie.value;

				if (cookie.secure)
					setCookieHeader += "; secure";
				if (cookie.httpOnly)
					setCookieHeader += "; httponly";
				if (cookie.samesite.size() > 0)
					setCookieHeader += "; samesite=" + cookie.samesite;

				response.setHeader("Set-Cookie", setCookieHeader);
			}
		}
	}
	return 0;
}

void Server::handleMultipart(Connection &connection)
{
	Request    &request = connection.request;
	std::string body = request.getBody();
	std::string contentType = request.getHeaders()["content-type"];
	std::string boundary =
	    "--" + contentType.substr(contentType.find("boundary=") + 9);
	std::string uploadPath = _config.getMainRoot() + _config.getUploadPath();

	if (!utils::pathExists(uploadPath))
		mkdir(uploadPath.c_str(), 0777);

	size_t pos = 0;
	size_t endPos;
	while ((pos = body.find(boundary, pos)) != std::string::npos)
	{
		pos += boundary.size();

		if (body.substr(pos, 2) == "--")
			break;
		pos += 2;
		endPos = body.find(boundary, pos);
		if (endPos == std::string::npos)
			endPos = body.size();
		std::string part = body.substr(pos, endPos - pos);
		if (part.find("filename=\"") != std::string::npos)
		{
			size_t      nameStart = part.find("filename=\"") + 10;
			size_t      nameEnd = part.find("\"", nameStart);
			std::string fileName = part.substr(nameStart, nameEnd - nameStart);
			size_t      fileStart = part.find("\r\n\r\n") + 4;
			size_t      fileEnd = part.rfind("\r\n");
			std::string fileContent = part.substr(fileStart, fileEnd - fileStart);
			std::string filePath = uploadPath + "/" + fileName;
			std::string fileReference = "Content-Disposition: form-data; name=\"" +
			                            fileName + "\"\r\n\r\n" + filePath + "\r\n";
			std::ofstream file(filePath.c_str());
			file << fileContent;
			file.close();
			body.replace(pos, endPos - pos, fileReference);
			pos += fileReference.size();
		}
		else
		{
			pos = endPos;
		}
	}
	request.updateBody(body);
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
