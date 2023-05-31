#include "Response.hpp"
#include "doctest.h"

TEST_CASE("Response class")
{
	Webserv::Response response;

	SUBCASE("Setters and getters")
	{
		response.setStatus(200);
		CHECK_EQ(response.getStatusCode(), 200);

		response.setHeader("Content-Type", "text/html");
		CHECK_EQ(response.getHeader("Content-Type"), "text/html");
		CHECK_EQ(response.getHeader("Unknown-Header"), "");

		response.setBody("<html><body>Hello, world!</body></html>");
		CHECK_EQ(response.getBody(), "<html><body>Hello, world!</body></html>");
	}

	SUBCASE("build_response method")
	{
		SUBCASE("Status code 404 with no body")
		{
			response.setStatus(404);
			std::string expected_response = "HTTP/1.1 404 Not Found\r\n"
			                                "\r\n";
			CHECK_EQ(response.buildResponse(), expected_response);
		}

		SUBCASE("Status code 200 with body")
		{
			response.setStatus(200);
			response.setHeader("Content-Type", "text/html");
			response.setHeader("Content-Length", "42");
			response.setBody("<html><body>Hello, world!!!!</body></html>");
			std::string expected_response = "HTTP/1.1 200 OK\r\n"
			                                "Content-Length: 42\r\n"
			                                "Content-Type: text/html\r\n"
			                                "\r\n"
			                                "<html><body>Hello, "
			                                "world!!!!</body></html>";
			CHECK_EQ(response.buildResponse(), expected_response);
		}
	}
}