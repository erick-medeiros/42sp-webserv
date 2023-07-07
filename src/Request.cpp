#include "Request.hpp"
#include <unistd.h>

Request::Request()
    : errorCode(0), contentLength(0), startLineParsed(false), headersParsed(false),
      bodyParsed(false)
{
}

Request::~Request(void){};

void Request::parse(std::string const rawInput, Config const &config)
{
	unparsed += rawInput;

	if (!startLineParsed)
	{
		parseStartLine(config);
		if (!startLineParsed)
			return;
	}
	log.error("contentLength: " + utils::to_string(contentLength));
	log.error("unparsed.size(): " + utils::to_string(unparsed.size()));
	log.error("getClientBodySize: " + utils::to_string(config.getClientBodySize()));
	if (contentLength > config.getClientBodySize())
	{
		errorCode = HttpStatus::PAYLOAD_TOO_LARGE;
		return;
	}
	if (!headersParsed)
	{
		parseHeaders();
		if (!headersParsed)
			return;
	}
	if (!bodyParsed)
	{
		parseBody(config);
		if (!bodyParsed)
			return;
	}
}

void Request::parseBody(Config const &config)
{
	// https://www.rfc-editor.org/rfc/rfc9112.html#section-6-4
	if (contentLength > 0)
	{
		if (unparsed.size() > config.getClientBodySize())
		{
			errorCode = HttpStatus::PAYLOAD_TOO_LARGE;
			return;
		}
		// Wait until unparsed has enough data to parse the body
		if (unparsed.size() < contentLength)
			return;

		body = unparsed.substr(0, contentLength);
		unparsed.erase(0, contentLength);
	}
	bodyParsed = true;
}

void Request::updateBody(std::string const &newBody)
{
	body = newBody;
}

void Request::parseStartLine(Config const &config)
{
	// Only parse if we have received the full start-line, delimited by \r\n
	std::size_t pos = unparsed.find("\r\n");
	if (pos == std::string::npos)
		return;
	std::string startLine = unparsed.substr(0, pos);
	unparsed = unparsed.substr(pos + 2); // Save the rest for parsing headers

	std::istringstream iss(startLine);
	std::string        method, uri, httpVersion;

	iss >> method;
	method = utils::trim(method);
	if (!isValidMethod(method, config))
	{
		this->errorCode = HttpStatus::NOT_IMPLEMENTED;
		return;
	}

	iss >> uri;
	uri = utils::trim(uri);
	if (uri.empty())
		throw std::runtime_error("missing request URL");

	iss >> httpVersion;
	httpVersion = utils::trim(httpVersion);
	if (httpVersion.empty())
		throw std::runtime_error("missing HTTP protocol version");
	if (!isValidHttpVersion(httpVersion))
	{
		this->errorCode = HttpStatus::
		    HTTP_VERSION_NOT_SUPPORTED; // https://www.rfc-editor.org/rfc/rfc9110#section-15.6.6
		throw std::runtime_error("invalid HTTP protocol version: " + httpVersion);
	}

	this->startLine["method"] = method;
	this->startLine["url"] = uri;
	this->startLine["version"] = httpVersion;

	std::string resource = uri;

	int offset = resource.find("?");
	if (offset != -1)
	{
		this->resourcePath = resource.substr(0, offset);

		std::string query = resource.substr(offset + 1);
		int         queryOffset = query.find("?");
		if (queryOffset != -1)
		{
			query = query.substr(0, queryOffset);
		}
		this->resourceQuery = query;
	}
	else
	{
		this->resourcePath = resource;
		this->resourceQuery = "";
	}

	startLineParsed = true;
}

void Request::parseHeaders()
{
	// Only parse if we have received the full headers section, delimited by \r\n\r\n
	if (unparsed.find("\r\n\r\n") == std::string::npos)
		return;

	std::istringstream iss(unparsed);
	std::string        row;

	while (std::getline(iss, row))
	{
		row = utils::trim(row);
		if (row.empty())
		{
			// End of headers section
			headersParsed = true;
			break;
		}

		std::string::size_type pos = row.find(":");
		if (pos == std::string::npos)
		{
			errorCode = HttpStatus::BAD_REQUEST;
			throw std::runtime_error("invalid header syntax: " + row);
		}

		std::string key = row.substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		key = utils::trim(key);
		std::string value = row.substr(pos + 1);
		value = utils::trim(value);

		if (value.empty())
		{
			errorCode = HttpStatus::BAD_REQUEST;
			throw std::runtime_error("invalid header value: " + row);
		}

		header[key] = value;

		// Handle Content-Length separately
		if (key == "content-length")
		{
			std::istringstream iss(value);
			iss >> contentLength;
			if (iss.fail())
			{
				errorCode = HttpStatus::BAD_REQUEST;
				throw std::runtime_error("invalid Content-Length value: " + value);
			}
		}
	}
	unparsed = iss.str().substr(iss.tellg()); // Keep the remaining unparsed part
}

bool Request::isMultipart() const
{
	if (header.find("content-type") != header.end())
	{
		std::string value = header.at("content-type");
		if (value.find("multipart/form-data") != std::string::npos)
			return true;
	}
	return false;
}

std::map<std::string, std::string> Request::getStartLine(void) const
{
	return this->startLine;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return this->header;
}

std::string Request::getBody(void) const
{
	return this->body;
}

std::string Request::getResourcePath(void) const
{
	return this->resourcePath;
}

std::string Request::getResourceQuery(void) const
{
	return this->resourceQuery;
}

std::vector<std::string> Request::getAllParams(void) const
{
	std::vector<std::string> params;
	std::stringstream        ss(this->resourceQuery);
	std::string              token;

	while (std::getline(ss, token, '&'))
	{
		if (ss.fail())
		{
			throw std::runtime_error("parsing query parameter");
		}
		params.push_back(token);
	}

	return params;
}

std::string Request::getMethod(void) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->startLine.find("method");
	if (it == this->startLine.end())
	{
		return "";
	}
	return it->second;
};

std::string Request::getUrl(void) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->startLine.find("url");
	if (it == this->startLine.end())
	{
		return "";
	}
	return it->second;
};

std::string Request::getHeaderValue(std::string const headerValue) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->header.find(headerValue);
	if (it != this->header.end())
	{
		return it->second;
	}
	return "";
};

bool Request::isValidMethod(std::string const &requestMethod,
                            Config const      &config) const
{
	std::vector<std::string> allowedMethods = config.getAllowedMethods();
	if (utils::contains(allowedMethods, requestMethod))
	{
		return true;
	}
	return false;
}

bool Request::isValidHttpVersion(std::string &requestVersion) const
{
	std::string const protocols[2] = {"HTTP/1.0", "HTTP/1.1"};

	int index = 0;
	for (int i = 0; i < 2 && requestVersion != protocols[i]; ++i)
	{
		index++;
	}

	switch (index)
	{
	case V10:
	case V11:
		return true;
	default:
		return false;
	}
}

bool Request::isParsed(void) const
{
	if (this->errorCode != 0)
	{
		return true;
	}
	return this->startLineParsed && this->headersParsed && this->bodyParsed;
}

void Request::setErrorCode(int errorCode)
{
	this->errorCode = errorCode;
}

int Request::getErrorCode(void) const
{
	return this->errorCode;
}

std::ostream &operator<<(std::ostream &os, Request const &req)
{
	os << CYAN "========== START LINE ==========" RES << std::endl;
	std::map<std::string, std::string> sl = req.getStartLine();
	for (std::map<std::string, std::string>::const_iterator it = sl.begin();
	     it != sl.end(); ++it)
	{
		os << GRAY << it->first << RES ": " BLUE << it->second << RES "\n";
	}

	os << CYAN "\n===========  HEADER  ===========" RES << std::endl;
	std::map<std::string, std::string> header = req.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = header.begin();
	     it != header.end(); ++it)
	{
		os << GRAY << it->first << RES ": " BLUE << it->second << RES "\n";
	}

	os << CYAN "\n============  BODY  ============" RES << std::endl;
	os << GREEN << req.getBody() << RES << std::endl;

	os << CYAN "\n============== EXTRA =============" RES << std::endl;
	std::vector<std::string> params = req.getAllParams();
	os << GRAY "Path:   " BLUE << req.getResourcePath() << RES << std::endl;
	os << GRAY "Params: " BLUE << req.getResourceQuery() << RES << std::endl;
	for (std::vector<std::string>::const_iterator it = params.begin();
	     it != params.end(); ++it)
	{
		os << YELLOW << *it << RES << std::endl;
	}
	return os << CYAN "==================================\n" RES << std::endl;
}