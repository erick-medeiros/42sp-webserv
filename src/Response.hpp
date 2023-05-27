#ifndef RESPONSE_H
#define RESPONSE_H

#include <map>
#include <string>
#include <sstream>

namespace Webserv {
    class Response {
    private:
        std::map<int, std::string> status_messages;
        std::map<std::string, std::string> headers;
        int status_code;
        std::string body;

        void initialize_status_messages();

    public:
        Response();
        void set_header(const std::string& key, const std::string& value);
        std::string get_header(const std::string& key) const;
        void set_status(int code);
        int get_status_code() const;
        std::string get_status_message(int code) const;
        std::string build_response() const;
        void set_body(const std::string& body);
        std::string get_body() const;
    };
}

#endif  // RESPONSE_H
