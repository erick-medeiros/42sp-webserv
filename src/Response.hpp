#ifndef RESPONSE_H
#define RESPONSE_H

#include <map>
#include <sstream>
#include <string>

#define HTTP_VERSION "HTTP/1.1"

class Response
{
  public:
	// Constructor
	Response();
	~Response();

	// Setters
	void setStatus(int code);
	void setHeader(const std::string &key, const std::string &value);
	void setBody(const std::string &body);

	// Getters
	int         getStatusCode() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;

	// Other methods
	void        loadFile(const std::string &path);
	void        sendTo(int socket) const;
	std::string buildResponse() const;

  private:
	// Attributes
	int                                statusCode;
	std::map<int, std::string>         reasonPhrase;
	std::map<std::string, std::string> headers;
	std::string                        body;
};

#endif
