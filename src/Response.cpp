#include "Response.hpp"

namespace Webserv {
    Response::Response() {
        // Initialize status messages
        status_messages[200] = "OK";
        status_messages[404] = "Not Found";
    }

    // -- Setters ---
    void Response::set_status(int code) {
        status_code = code;
    }

    void Response::set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    void Response::set_body(const std::string& body) {
        this->body = body;
    }

    // --- Getters --- 
    int Response::get_status_code() const {
        return status_code;
    }

    std::string Response::get_status_message(int code) const {
        std::map<int, std::string>::const_iterator it = status_messages.find(code);
        if (it != status_messages.end()) {
            return it->second;
        } 
        return "";
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

    // --- Other methods ---
    std::string Response::build_response() const {
        std::stringstream response;
        // Add status line
        response << "HTTP/1.1 " << status_code << " " << get_status_message(status_code) << "\r\n";
        // Add headers
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response << it->first << ": " << it->second << "\r\n";
        }
        // Add body
        response << "\r\n" << body << "\r\n";
        // Return response
        return response.str();
    }
}
