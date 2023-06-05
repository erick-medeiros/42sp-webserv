#include "Response.hpp"

#include <fstream>      // ifstream
#include <sstream>      // stringstream
#include <sys/socket.h> // send

void Response::loadFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		// Set status code to 404
		setStatus(404);
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	setBody(buffer.str());
	file.close();
}

void Response::sendTo(int socket) const
{
	std::string response = buildResponse();
	if (send(socket, response.c_str(), response.size(), 0) < 0)
	{
		throw std::runtime_error("Error sending response");
	}
}

Response::Response() : statusCode(200)
{
	// Initialize status messages
	reasonPhrase[200] = "OK";
	reasonPhrase[404] = "Not Found";
}

Response::~Response(){};

// -- Setters ---
void Response::setStatus(int code)
{
	// Don't allow a status that it not mapped
	if (reasonPhrase.find(code) == reasonPhrase.end())
		throw std::invalid_argument("Invalid status code");
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

std::string Response::buildResponse() const
{
	std::stringstream response;
	// Status line
	response << HTTP_VERSION << " " << statusCode << " "
	         << reasonPhrase.at(statusCode) << "\r\n";
	// Headers
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	     it != headers.end(); ++it)
	{
		response << it->first << ": " << it->second << "\r\n";
	}
	// Empty line indicating the end of the header section
	response << "\r\n";
	// Optional message body
	if (body.size() > 0)
		response << body;
	return response.str();
}