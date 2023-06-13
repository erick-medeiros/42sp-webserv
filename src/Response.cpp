#include "Response.hpp"

#include <dirent.h>     // opendir
#include <fstream>      // ifstream
#include <sstream>      // stringstream
#include <sys/socket.h> // send

#include "log_utils.hpp"

// TODO: Move this
int is_dir(const std::string &path)
{
	DIR *dir = opendir(path.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		return 1;
	}
	return 0;
}

void Response::loadFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open() || is_dir(path))
	{
		setStatus(404);
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	setBody(buffer.str());
	file.close();
}

Response::Response(const Request &request) : statusCode(200)
{
	this->clientFd = request.getFd();
	if (request.isCgiEnabled())
	{
		std::stringstream ss;
		ss << this->clientFd;
		std::string const tempFile(CGI_RESPONSE + ss.str());
		loadFile(tempFile);
		std::remove(tempFile.c_str());
	}
	else
	{
		parse(request);
	}
}

void Response::parse(const Request &request)
{
	if (request.getMethod() == "GET")
	{
		// TODO: Trocar esse files pelo root_path passado na conf
		std::string filePath = "files" + request.getResourcePath();
		loadFile(filePath);
	}
	// TODO: Implementar outros métodos além do GET
}

Response::Response() : statusCode(200) {}

Response::~Response(){};

// -- Setters ---
void Response::setStatus(int code)
{
	this->statusCode = code;
}

void Response::setHeader(const std::string &key, const std::string &value)
{
	headers[key] = value;
}

void Response::setBody(const std::string &body)
{
	std::stringstream string_stream;
	string_stream << body.size();
	setHeader("Content-Length", string_stream.str());
	this->body = body;
}

// --- Getters ---
int Response::getStatusCode() const
{
	return statusCode;
}

std::string Response::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end())
	{
		return it->second;
	}
	return "";
}

std::string Response::getBody() const
{
	return body;
}

std::string Response::getMessage(void) const
{
	return this->message;
}

// https://www.rfc-editor.org/rfc/rfc9112#section-2.1
void Response::prepareMessage()
{
	std::stringstream message;

	// Status line
	message << HTTP_VERSION << SP << statusCode << SP << getReasonPhrase() << CRLF;

	// Headers
	typedef std::map<std::string, std::string> HeadersMap;
	for (HeadersMap::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		message << it->first << HEADER_SEPARATOR << it->second << CRLF;
	}

	// Empty line indicating the end of the header section
	message << CRLF;

	// Optional message body
	if (this->body.size() > 0)
		message << this->body;

	this->message = message.str();
}

void Response::sendHttpResponse()
{
	prepareMessage();
	logWarning("Sending response:\n" + message);

	if (send(clientFd, message.c_str(), message.size(), 0) < 0)
	{
		throw std::runtime_error("Error sending response");
	}
}

// --- Reason Phrase
std::string Response::getReasonPhrase(void) const
{
	switch (statusCode)
	{
	case 100:
		return "Continue";
	case 101:
		return "Switching Protocols";
	case 102:
		return "Processing";
	case 200:
		return "OK";
	case 201:
		return "Created";
	case 202:
		return "Accepted";
	case 203:
		return "Non-Authoritative Information";
	case 204:
		return "No Content";
	case 205:
		return "Reset Content";
	case 206:
		return "Partial Content";
	case 207:
		return "Multi-Status";
	case 208:
		return "Already Reported";
	case 226:
		return "IM Used";
	case 300:
		return "Multiple Choices";
	case 301:
		return "Moved Permanently";
	case 302:
		return "Found";
	case 303:
		return "See Other";
	case 304:
		return "Not Modified";
	case 305:
		return "Use Proxy";
	case 307:
		return "Temporary Redirect";
	case 308:
		return "Permanent Redirect";
	case 400:
		return "Bad Request";
	case 401:
		return "Unauthorized";
	case 402:
		return "Payment Required";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	case 405:
		return "Method Not Allowed";
	case 406:
		return "Not Acceptable";
	case 407:
		return "Proxy Authentication Required";
	case 408:
		return "Request Timeout";
	case 409:
		return "Conflict";
	case 410:
		return "Gone";
	case 411:
		return "Length Required";
	case 412:
		return "Precondition Failed";
	case 413:
		return "Payload Too Large";
	case 414:
		return "URI Too Long";
	case 415:
		return "Unsupported Media Type";
	case 416:
		return "Range Not Satisfiable";
	case 417:
		return "Expectation Failed";
	case 421:
		return "Misdirected Request";
	case 422:
		return "Unprocessable Entity";
	case 423:
		return "Locked";
	case 424:
		return "Failed Dependency";
	case 426:
		return "Upgrade Required";
	case 428:
		return "Precondition Required";
	case 429:
		return "Too Many Requests";
	case 431:
		return "Request Header Fields Too Large";
	case 451:
		return "Unavailable For Legal Reasons";
	case 500:
		return "Internal Server Error";
	case 501:
		return "Not Implemented";
	case 502:
		return "Bad Gateway";
	case 503:
		return "Service Unavailable";
	case 504:
		return "Gateway Timeout";
	case 505:
		return "HTTP Version Not Supported";
	case 506:
		return "Variant Also Negotiates";
	case 507:
		return "Insufficient Storage";
	case 508:
		return "Loop Detected";
	case 510:
		return "Not Extended";
	case 511:
		return "Network Authentication Required";
	default:
		return "Unknown";
	}
}