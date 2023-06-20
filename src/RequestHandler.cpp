#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Config &conf) : _config(conf) {}

Response RequestHandler::handle(const Request &request)
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

	// -- EARLY RETURN CHECKS --
	
	// Config defined a specific return code
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/301
	if (_config.hasReturn(requestPath))
	{
		response.setStatus(_config.getReturnCode(requestPath));
		response.setHeader("Location", _config.getReturnLocation(requestPath));
		return response;
	}

	// Request host is not in the server names
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

	// If file doesn't exist, return 404
	if (!utils::fileExists(filePath))
	{
		response.setStatus(HttpStatus::NOT_FOUND);
		return response;
	}

	// --- END EARLY RETURN CHECKS ---

	// Request asked for a CGI script
	if (_config.isCGI(requestPath))
	{
		std::string scriptOutput = executeScript(requestPath);
		response.setBody(scriptOutput);
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

	// If request body is too large, return 413
	if (request.getBody().size() > _config.getClientBodySize())
	{
		response.setStatus(HttpStatus::PAYLOAD_TOO_LARGE);
	}

	return response;
}
