#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "utils.hpp"

enum
{
	GET,
	POST,
	DELETE
};

enum
{
	V10,
	V11
};

class Request
{
  private:
	Request(void);
	std::map<std::string, std::string> startLine;
	std::map<std::string, std::string> header;
	std::string                        body;
	std::string                        resourcePath;
	std::string                        resourceQuery;
	void                               initStartLine(std::istringstream &iss);
	void                               initHeaders(std::istringstream &iss);
	void                               parseURL(void);

  public:
	Request(std::string const req);
	~Request(void);

	std::map<std::string, std::string> getStartLine(void) const;
	std::map<std::string, std::string> getHeaders(void) const;
	std::string                        getBody(void) const;
	std::string                        getResourcePath(void) const;
	std::string                        getResourceQuery(void) const;
	std::vector<std::string>           getAllParams(void) const;
	std::string                        getMethod(void) const;
	std::string                        getUrl(void) const;
	std::string                        getHeaderValue(std::string const) const;
	bool isValidMethod(std::string const &requestMethod) const;
	bool isValidHttpVersion(std::string &requestVersion) const;
};

std::ostream &operator<<(std::ostream &os, Request const &req);

#endif
