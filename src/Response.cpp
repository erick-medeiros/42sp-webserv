#include "Response.hpp"

const std::string Webserv::Response::HTTP_VERSION = "HTTP/1.1";

namespace Webserv {
    Response::Response() {
        // Initialize status messages
        reason_phrase[200] = "OK";
        reason_phrase[404] = "Not Found";
    }

    // -- Setters ---
    void Response::set_status(int code) {
        // Don't allow a status that it not mapped
        if (reason_phrase.find(code) == reason_phrase.end())
            throw std::invalid_argument("Invalid status code");
        this->status_code = code;
    }

    void Response::set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    void Response::set_body(const std::string& body) {
        std::stringstream string_stream;
        string_stream << body.size();
        set_header("Content-Length", string_stream.str());
        this->body = body;
    }

    // --- Getters --- 
    int Response::get_status_code() const {
        return status_code;
    }

    std::string Response::get_header(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator it = headers.find(key);
        if (it != headers.end()) {
            return it->second;
        }
        return "";
    }

    std::string Response::get_body() const {
        return body;
    }

    std::string Response::build_response() const {
        std::stringstream response;
        // Status line
        response << HTTP_VERSION << " " << status_code << " " << reason_phrase.at(status_code) << "\r\n";
        // Headers
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response << it->first << ": " << it->second << "\r\n";
        }
        // Empty line indicating the end of the header section
        response << "\r\n";
        // Optional message body
        if (body.size() > 0)
            response << body;
        return response.str();
    }
} 