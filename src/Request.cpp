#include "Request.hpp"

std::string &trim(std::string &s);

Request::Request(void){};

Request::Request(std::string const req)
{
	std::istringstream iss(req);

	initStartLine(iss);
	initHeaders(iss);
	parseURL();
};

Request::~Request(void){};

void Request::initStartLine(std::istringstream &iss)
{
	std::string row;

	std::getline(iss, row);
	if (iss.fail())
	{
		throw std::runtime_error("parsing request start-line");
	}

	std::stringstream ss(row);
	std::string       token;

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("parsing request method");
	}
	this->startLine["Method"] = trim(token);

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("parsing request resource");
	}
	this->startLine["URL"] = trim(token);

	ss >> token;
	if (ss.fail())
	{
		throw std::runtime_error("parsing request version");
	}
	this->startLine["Version"] = trim(token);
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
void Request::displayInfo(void) const
{
	std::cout << CYAN "========== START LINE ==========" RES << std::endl;
	std::map<std::string, std::string> sl = this->getStartLine();
	for (std::map<std::string, std::string>::const_iterator it = sl.begin();
	     it != sl.end(); ++it)
	{
		std::cout << GRAY << it->first << RES ": " BLUE << it->second
		          << RES "\n";
	}

	std::cout << CYAN "\n===========  HEADER  ===========" RES << std::endl;
	std::map<std::string, std::string> header = this->getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = header.begin();
	     it != header.end(); ++it)
	{
		std::cout << GRAY << it->first << RES ": " BLUE << it->second
		          << RES "\n";
	}

	std::cout << CYAN "\n============  BODY  ============" RES << std::endl;
	std::cout << GREEN << this->getBody() << RES << std::endl;

	std::cout << CYAN "\n============== EXTRA =============" RES << std::endl;
	std::vector<std::string> params = this->getAllParams();
	std::cout << GRAY "Path:   " BLUE << this->getResourcePath() << RES
	          << std::endl;
	std::cout << GRAY "Params: " BLUE << this->getResourceQuery() << RES
	          << std::endl;
	for (std::vector<std::string>::const_iterator it = params.begin();
	     it != params.end(); ++it)
	{
		std::cout << YELLOW << *it << RES << std::endl;
	}
	std::cout << CYAN "==================================" RES << std::endl;
}

// getters
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

// utils
static std::string &rtrim(std::string &s, const char *t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

static std::string &ltrim(std::string &s, const char *t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::string &trim(std::string &s)
{
	const char *ws = " \t\n\r\f\v";
	return ltrim(rtrim(s, ws), ws);
}
