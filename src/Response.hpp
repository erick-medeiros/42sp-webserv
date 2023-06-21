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
	Response(const Request &request);
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
	void        loadFile(const std::string &path);
	void        listDir(const std::string &path);
	void        prepareMessage(void);
	void        sendHttpResponse(void);
	std::string getReasonPhrase(void) const;

  private:
	// Attributes
	int                                statusCode;
	int                                clientFd;
	std::map<std::string, std::string> headers;
	std::string                        body;
	std::string                        message;
};

#endif
