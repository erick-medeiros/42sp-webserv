#include "doctest.h"
#include "Response.hpp"

TEST_CASE("Response test") {
    Webserv::Response response;

    SUBCASE("Setting and retrieving headers") {
        response.set_header("Content-Type", "text/html");
        response.set_header("Content-Length", "42");

        CHECK_EQ(response.get_header("Content-Type"), "text/html");
        CHECK_EQ(response.get_header("Content-Length"), "42");
        CHECK_EQ(response.get_header("Unknown-Header"), "");
    }

    SUBCASE("Setting and retrieving status code") {
        response.set_status(200);

        CHECK_EQ(response.get_status_code(), 200);
    }

    SUBCASE("Getting status message") {
        CHECK_EQ(response.get_status_message(200), "OK");
        CHECK_EQ(response.get_status_message(404), "Not Found");
        CHECK_EQ(response.get_status_message(500), "");
    }

    SUBCASE("Building response") {
        response.set_status(200);
        response.set_header("Content-Type", "text/html");
        response.set_body("<html><body>Hello, world!</body></html>");

        std::string expected_response = "HTTP/1.1 200 OK\r\n"
                                        "Content-Type: text/html\r\n"
                                        "\r\n"
                                        "<html><body>Hello, world!</body></html>\r\n";

        CHECK_EQ(response.build_response(), expected_response);
    }
}
