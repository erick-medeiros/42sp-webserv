#ifndef RESPONSE_H
#define RESPONSE_H

#include <map>
#include <string>
#include <sstream>

namespace Webserv {
    class Response {
    public:
        // Constructor
        Response();

        // Setters
        void set_status(int code);
        void set_header(const std::string& key, const std::string& value);
        void set_body(const std::string& body);

        // Getters
        int get_status_code() const;
        std::string get_status_message(int code) const;
        std::string get_header(const std::string& key) const;
        std::string get_body() const;

        // Other methods
        std::string build_response() const;

    private:
        // Attributes
        int status_code;
        std::map<int, std::string> status_messages;
        std::map<std::string, std::string> headers;
        std::string body;
    };
}

#endif
