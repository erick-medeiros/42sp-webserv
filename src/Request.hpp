#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "HttpStatus.hpp"
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
	int                                fd;
	int                                errorCode;
	std::map<std::string, std::string> startLine;
	std::map<std::string, std::string> header;
	std::string                        body;
	std::string                        resourcePath;
	std::string                        resourceQuery;
	std::string                        unparsed;
	void                               parseStartLine(std::istringstream &iss);
	void                               parseURI();
	void                               parseHeaders(std::istringstream &iss);
	void                               parseBody(std::istringstream &iss);
	bool                               startLineParsed;
	bool                               headersParsed;
	bool                               URIParsed;
	bool                               bodyParsed;
	bool                               cgiState;

  public:
	Request(int fd);
	~Request(void);
	void parse(std::string const rawRequest);

	std::map<std::string, std::string> getStartLine(void) const;
	std::map<std::string, std::string> getHeaders(void) const;
	std::string                        getBody(void) const;
	std::string                        getResourcePath(void) const;
	std::string                        getResourceQuery(void) const;
	std::vector<std::string>           getAllParams(void) const;
	std::string                        getMethod(void) const;
	std::string                        getUrl(void) const;
	std::string                        getHeaderValue(std::string const) const;
	int                                getFd(void) const;
	bool isValidMethod(std::string const &requestMethod) const;
	bool isValidHttpVersion(std::string &requestVersion) const;
	bool isCgiEnabled(void) const;
	void setCgiAs(bool newState);
};

std::ostream &operator<<(std::ostream &os, Request const &req);

#endif
