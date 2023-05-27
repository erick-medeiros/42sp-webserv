#include "response.hpp"

namespace Webserv {
    Response::Response() {
        initialize_status_messages();
    }

    void Response::initialize_status_messages() {
        // Initialize status code and messages
        status_messages[200] = "OK";
        status_messages[404] = "Not Found";
        // Add more status codes and messages as needed
    }

    void Response::set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    std::string Response::get_header(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator it = headers.find(key);
        if (it != headers.end()) {
            return it->second;
        } else {
            return ""; // Return an empty string if key is not found
        }
    }

    void Response::set_status(int code) {
        status_code = code;
    }

    int Response::get_status_code() const {
        return status_code;
    }

    std::string Response::get_status_message(int code) const {
        std::map<int, std::string>::const_iterator it = status_messages.find(code);
        if (it != status_messages.end()) {
            return it->second;
        } else {
            return ""; // Return an empty string if code is not found
        }
    }

    std::string Response::build_response() const {
        std::stringstream response;
        response << "HTTP/1.1 " << status_code << " " << get_status_message(status_code) << "\r\n";
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response << it->first << ": " << it->second << "\r\n";
        }
        response << "\r\n" << body << "\r\n";
        return response.str();
    }

    void Response::set_body(const std::string& body) {
        this->body = body;
    }

    std::string Response::get_body() const {
        return body;
    }
}
