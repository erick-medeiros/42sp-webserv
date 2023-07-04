#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "HttpStatus.hpp"
#include "Logger.hpp"
#include "log_utils.hpp"
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
	int                                errorCode;
	int                                contentLength;
	std::map<std::string, std::string> startLine;
	std::map<std::string, std::string> header;
	std::string                        body;
	std::string                        resourcePath;
	std::string                        resourceQuery;
	std::string                        unparsed;
	void                               parseStartLine();
	void                               parseHeaders();
	void                               parseBody();
	bool                               startLineParsed;
	bool                               headersParsed;
	bool                               URIParsed;
	bool                               bodyParsed;

  public:
	Request(void);
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
	bool isValidMethod(std::string const &requestMethod) const;
	bool isValidHttpVersion(std::string &requestVersion) const;
	bool isParsed(void) const;
	bool isMultipart(void) const;
	void setErrorCode(int errorCode);
	void updateBody(std::string const &newBody);
};

std::ostream &operator<<(std::ostream &os, Request const &req);

#endif
