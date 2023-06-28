#ifndef RESPONSE_H
#define RESPONSE_H

#include <map>
#include <sstream>
#include <string>

#include "Request.hpp"

#define HTTP_VERSION "HTTP/1.1"
#define CRLF "\r\n"
#define SP " "
#define HEADER_SEPARATOR ": "

class Response
{
  public:
	// Constructor
	Response();
	~Response();

	bool cgiState;

	// Setters
	void setStatus(int code);
	void setHeader(const std::string &key, const std::string &value);
	void setBody(const std::string &body);

	// Getters
	int         getStatusCode() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;
	std::string getMessage() const;

	// Other methods
	void        parse(const Request &request);
	void        loadFile(const std::string &path);
	void        listDir(const std::string &path);
	void        prepareMessage(void);
	std::string getReasonPhrase(void) const;
	void        setCustomErrorPage(int statusCode, const std::string &path);
	void        createWelcomePage();

  private:
	// Attributes
	int                                statusCode;
	std::map<std::string, std::string> headers;
	std::string                        body;
	std::string                        message;
	std::map<int, std::string>         customErrorPages;

	// Methods
	void addContentType(const std::string &filePath);
	void createErrorPage();
};

#endif
