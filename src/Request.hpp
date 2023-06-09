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

#include "Config.hpp"
#include "HttpStatus.hpp"
#include "Logger.hpp"
#include "utils.hpp"

enum
{
	V10,
	V11
};

class Request
{
  private:
	int                                errorCode;
	size_t                             contentLength;
	std::map<std::string, std::string> startLine;
	std::map<std::string, std::string> header;
	std::string                        body;
	std::string                        resourcePath;
	std::string                        resourceQuery;
	std::string                        unparsed;
	void                               parseStartLine(Config const &);
	void                               parseHeaders();
	void                               parseBody(Config const &config);
	bool                               startLineParsed;
	bool                               headersParsed;
	bool                               URIParsed;
	bool                               bodyParsed;

  public:
	Request(void);
	~Request(void);

	void parse(std::string const rawRequest, Config const &);
	std::map<std::string, std::string> getStartLine(void) const;
	std::map<std::string, std::string> getHeaders(void) const;
	std::string                        getBody(void) const;
	std::string                        getResourcePath(void) const;
	std::string                        getResourceQuery(void) const;
	std::vector<std::string>           getAllParams(void) const;
	std::string                        getMethod(void) const;
	std::string                        getUrl(void) const;
	std::string                        getHeaderValue(std::string const) const;
	int                                getErrorCode(void) const;
	bool isValidMethod(std::string const &, Config const &) const;
	bool isValidHttpVersion(std::string &requestVersion) const;
	bool isParsed(void) const;
	bool isMultipart(void) const;
	void setErrorCode(int errorCode);
	void updateBody(std::string const &newBody);
	bool hasCookies(void) const;

	// cookie {
	std::string getValueCookie(std::string const &key) const;
	std::string getNewCookieValue() const;
	// } cookie

	std::string getParam(const std::string &name) const;
};

std::ostream &operator<<(std::ostream &os, Request const &req);

#endif
