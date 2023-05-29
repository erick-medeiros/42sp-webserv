#include "doctest.h"
#include "Response.hpp"

TEST_CASE("Response class") {
    Webserv::Response response;

    SUBCASE("Setters and getters") {
        response.set_status(200);
        CHECK_EQ(response.get_status_code(), 200);

        response.set_header("Content-Type", "text/html");
        CHECK_EQ(response.get_header("Content-Type"), "text/html");
        CHECK_EQ(response.get_header("Unknown-Header"), "");

        response.set_body("<html><body>Hello, world!</body></html>");
        CHECK_EQ(response.get_body(), "<html><body>Hello, world!</body></html>");
    }

    SUBCASE("build_response method") {

        SUBCASE("Status code 404 with no body") {
            response.set_status(404);
            std::string expected_response = 
                "HTTP/1.1 404 Not Found\r\n"
                "\r\n";
            CHECK_EQ(response.build_response(), expected_response);
        }

        SUBCASE("Status code 200 with body") {
            response.set_status(200);
            response.set_header("Content-Type", "text/html");
            response.set_header("Content-Length", "42");
            response.set_body("<html><body>Hello, world!!!!</body></html>");
            std::string expected_response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 42\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<html><body>Hello, world!!!!</body></html>";
            CHECK_EQ(response.build_response(), expected_response);
        }
    }
}