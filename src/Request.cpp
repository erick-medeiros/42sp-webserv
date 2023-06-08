#include "Request.hpp"

Request::Request(int fd) : fd(fd){};

Request::~Request(void){};

void Request::parse(std::string const rawInput)
{
	std::istringstream iss(rawInput);

	initStartLine(iss);
	initHeaders(iss);
	parseURL();
	// TODO: Ler o body caso tenha um content-length no header
	// TODO: Depois de ler tudo que precisa, dizer que o request está finished
}

void Request::initStartLine(std::istringstream &iss)
{
	std::string row;

	std::getline(iss, row);
	if (iss.fail() || row.empty())
	{
		throw std::runtime_error("missing request start-line data");
	}

	std::stringstream ss(row);
	std::string       token;

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("missing HTTP request method");
	}
	if (isValidMethod(token))
	{
		this->startLine["Method"] = trim(token);
	}
	else
	{
		throw std::runtime_error("non supported HTTP method: " + token);
	}

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("missing request URL");
	}
	this->startLine["URL"] = trim(token);

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("missing HTTP protocol version");
	}
	if (isValidHttpVersion(token))
	{
		this->startLine["Version"] = trim(token);
	}
	else
	{
		throw std::runtime_error("invalid HTTP protocol version: " + token);
	}
}

void Request::initHeaders(std::istringstream &iss)
{
	std::string       row;
	std::string       key;
	std::string       value;
	std::stringstream ss;
	bool              isInvalidHeader = false;

	while (std::getline(iss, row))
	{
		if (iss.fail())
		{
			throw std::runtime_error("parsing request header");
		}
		row = trim(row);
		if (row.size() == 0)
		{
			ss << iss.rdbuf();
			break;
		}

		int offset = row.find(":");
		if (offset < 0)
		{
			isInvalidHeader = true;
			break;
		}
		key = row.substr(0, offset);

		value = std::string(row.begin() + offset + 1, row.end());
		value = trim(value);
		if (value.empty())
		{
			throw std::runtime_error("invalid header value");
		}
		this->header[trim(key)] = value;
	}
	if (!iss.eof())
	{
		this->body = ss.str();
	}
	else
	{
		this->body = "";
	}

	if (isInvalidHeader)
	{
		throw std::runtime_error("invalid header syntax");
	}
}

void Request::parseURL(void)
{
	std::string resource = this->startLine["URL"];

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
}

// debug

// getters
int Request::getFd(void) const
{
	return this->fd;
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
	    this->startLine.find("Method");
	if (it == this->startLine.end())
	{
		return "";
	}
	return it->second;
};

std::string Request::getUrl(void) const
{
	std::map<std::string, std::string>::const_iterator it =
	    this->startLine.find("URL");
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
	throw std::runtime_error("request header not found");
};

bool Request::isValidMethod(std::string const &requestMethod) const
{
	int const         numberOfAvaibleMethods = 3;
	std::string const methods[numberOfAvaibleMethods] = {"GET", "POST", "DELETE"};

	int index = 0;
	for (int i = 0; requestMethod != methods[i] && i < numberOfAvaibleMethods; ++i)
	{
		index++;
	}

	switch (index)
	{
	case GET:
	case POST:
	case DELETE:
		return true;
	default:
		return false;
	}
}

bool Request::isValidHttpVersion(std::string &requestVersion) const
{
	std::string const protocols[2] = {"HTTP/1.0", "HTTP/1.1"};

	int index = 0;
	for (int i = 0; requestVersion != protocols[i] && i < 2; ++i)
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
