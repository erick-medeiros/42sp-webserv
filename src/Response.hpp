#ifndef RESPONSE_H
#define RESPONSE_H

#include <ctime>    // time
#include <dirent.h> // opendir
#include <fstream>  // ifstream
#include <map>
#include <sstream> // stringstream
#include <string>
#include <sys/socket.h> // send
#include <sys/stat.h>   // stat

#include "Request.hpp"
#include "utils.hpp"

#define HTML_ROOT "." // TODO: Trocar pelo root_path passado na conf
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
	void        loadFile(const std::string &path);
	void        listDir(const std::string &root, std::string path);
	void        prepareMessage(void);
	std::string getReasonPhrase(void) const;
	void        setCustomErrorPage(int statusCode, const std::string &path);

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
