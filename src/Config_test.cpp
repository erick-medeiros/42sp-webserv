/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/06 19:52:17 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Logger.hpp"
#include "doctest.h"
#include "utils.hpp"
#include <sys/types.h>

TEST_SUITE("Class Config")
{
	TEST_CASE("Constructor")
	{
		Logger::level = LOGGER_LEVEL_NONE;
		Config conf;
	}
	TEST_CASE("error name")
	{
		Config config;
		freopen("/dev/null", "w", stderr);
		CHECK_EQ(config.add("ports", "8080"), 1);
		freopen("/dev/tty", "w", stderr);
	}
	TEST_CASE("valid config")
	{
		Config config;
		CHECK_FALSE(config.isValid());
		CHECK_EQ(config.add("port", "8080"), 0);
		CHECK(config.isValid());
	}
}

TEST_SUITE("ports")
{
	TEST_CASE("single port")
	{
		Config config;
		CHECK_EQ(config.add("port", "8080"), 0);
		uint_t const &port = config.getPort();
		CHECK_EQ(port, 8080);
	}
	TEST_CASE("multiple ports")
	{
		Config config;
		freopen("/dev/null", "w", stderr);
		CHECK_EQ(config.add("port", "8080 8090 9000"), 1);
		freopen("/dev/tty", "w", stderr);
		uint_t const &port = config.getPort();
		CHECK_EQ(port, 0);
	}
	TEST_CASE("limits")
	{
		Config config;
		freopen("/dev/null", "w", stderr);
		CHECK_EQ(config.add("port", "1023"), 1);
		CHECK_EQ(config.add("port", "49152"), 1);
		freopen("/dev/tty", "w", stderr);
		uint_t const &port = config.getPort();
		CHECK_EQ(port, 0);
		SUBCASE("min")
		{
			Config config;
			CHECK_EQ(config.add("port", "1024"), 0);
		}
		SUBCASE("max")
		{
			Config config;
			CHECK_EQ(config.add("port", "49151"), 0);
		}
	}
}

TEST_SUITE("server name")
{
	TEST_CASE("single")
	{
		Config      config;
		std::string server = "localhost";
		CHECK_EQ(config.add("server_name", server), 0);
		std::vector<std::string> const &serverNames = config.getServerNames();

		CHECK_EQ(serverNames.size(), 1);
		CHECK_EQ(serverNames[0], server);

		SUBCASE("error")
		{
			freopen("/dev/null", "w", stderr);
			CHECK_EQ(config.add("server_name", server), 1);
			freopen("/dev/tty", "w", stderr);
		}
	}

	TEST_CASE("multiples")
	{
		Config      config;
		std::string server1 = "localhost";
		std::string server2 = "www.localhost";

		CHECK_EQ(config.add("server_name", server1 + " " + server2), 0);

		{ // error
			freopen("/dev/null", "w", stderr);
			CHECK_EQ(config.add("server_name", server2), 1);
			freopen("/dev/tty", "w", stderr);
		}

		std::vector<std::string> const &serverNames = config.getServerNames();

		CHECK_EQ(serverNames.size(), 2);
		CHECK_EQ(serverNames[0], server1);
		CHECK_EQ(serverNames[1], server2);
	}

	TEST_CASE("bad url")
	{
		// TODO: make
	}
}

TEST_SUITE("error pages")
{
	TEST_CASE("add")
	{
		Config config;

		CHECK_EQ(config.add("error_page", "404 /404.html"), 0);
		CHECK_EQ(config.add("error_page", "405 /405.html"), 0);

		CHECK_EQ(config.getErrorPage(404), "/404.html");
		CHECK_EQ(config.getErrorPage(405), "/405.html");
		CHECK_EQ(config.getErrorPage(200), "");
	}

	TEST_CASE("error")
	{
		Config config;
		CHECK_EQ(config.add("error_page", "404 /404.html err"), 1);
	}

	TEST_CASE("range error")
	{
		Config config;
		freopen("/dev/null", "w", stderr);
		CHECK_EQ(config.add("error_page", "200 /200.html"), 1);
		CHECK_EQ(config.add("error_page", "399 /399.html"), 1);
		CHECK_EQ(config.add("error_page", "400 /400.html"), 0);
		CHECK_EQ(config.add("error_page", "599 /599.html"), 0);
		CHECK_EQ(config.add("error_page", "600 /600.html"), 1);
		CHECK_EQ(config.add("error_page", "900 /900.html"), 1);
		freopen("/dev/tty", "w", stderr);

		CHECK_EQ(config.getErrorPage(200), "");
		CHECK_EQ(config.getErrorPage(400), "/400.html");
		CHECK_EQ(config.getErrorPage(599), "/599.html");
		CHECK_EQ(config.getErrorPage(900), "");
	}
}

TEST_SUITE("client_max_body_size")
{
	TEST_CASE("default")
	{
		Config config;
		CHECK_EQ(config.getClientBodySize(), 1024 * 1024);
	}

	TEST_CASE("mb")
	{
		Config config;
		CHECK_EQ(config.add("client_max_body_size", "1M"), 0);
		CHECK_EQ(config.getClientBodySize(), 1024 * 1024);
	}

	TEST_CASE("kb")
	{
		Config config;
		CHECK_EQ(config.add("client_max_body_size", "1K"), 0);
		CHECK_EQ(config.getClientBodySize(), 1024);
	}

	TEST_CASE("errors")
	{
		Config config;

		CHECK_EQ(config.add("client_max_body_size", "0"), 1);
		CHECK_EQ(config.add("client_max_body_size", "1"), 1);
		CHECK_EQ(config.add("client_max_body_size", ""), 1);
		CHECK_EQ(config.add("client_max_body_size", "1B"), 1);
		CHECK_EQ(config.add("client_max_body_size", "1m"), 1);
	}
}

TEST_SUITE("allowed_methods")
{
	TEST_CASE("default")
	{
		Config config;

		std::vector<std::string> allowedMethods = config.getAllowedMethods();

		CHECK_EQ(allowedMethods.size(), 3);
		CHECK_EQ(allowedMethods[0], "GET");
		CHECK_EQ(allowedMethods[1], "POST");
		CHECK_EQ(allowedMethods[2], "DELETE");
	}

	TEST_CASE("GET only")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", "GET"), 0);

		std::vector<std::string> allowedMethods = config.getAllowedMethods();

		CHECK_EQ(allowedMethods.size(), 1);
		CHECK_EQ(allowedMethods[0], "GET");
	}

	TEST_CASE("GET and POST")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", "GET POST"), 0);

		std::vector<std::string> allowedMethods = config.getAllowedMethods();

		CHECK_EQ(allowedMethods.size(), 2);
		CHECK_EQ(allowedMethods[0], "GET");
		CHECK_EQ(allowedMethods[1], "POST");
	}

	TEST_CASE("all")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);

		std::vector<std::string> allowedMethods = config.getAllowedMethods();

		CHECK_EQ(allowedMethods.size(), 3);
		CHECK_EQ(allowedMethods[0], "GET");
		CHECK_EQ(allowedMethods[1], "POST");
		CHECK_EQ(allowedMethods[2], "DELETE");
	}

	TEST_CASE("duplicated method")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", "GET GET"), 0);
		std::vector<std::string> allowedMethods1 = config.getAllowedMethods();
		CHECK_EQ(allowedMethods1.size(), 1);

		CHECK_EQ(config.add("allowed_methods", "GET POST GET"), 0);
		std::vector<std::string> allowedMethods2 = config.getAllowedMethods();
		CHECK_EQ(allowedMethods2.size(), 2);

		CHECK_EQ(config.add("allowed_methods", "GET POST DELETE GET GET POST"), 0);
		std::vector<std::string> allowedMethods3 = config.getAllowedMethods();
		CHECK_EQ(allowedMethods3.size(), 3);
	}

	TEST_CASE("empty")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", ""), 1);
	}

	TEST_CASE("invalid HTTP method")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", "FOO"), 1);
	}

	TEST_CASE("not required methods")
	{
		Config config;

		CHECK_EQ(config.add("allowed_methods", "OPTIONS"), 1);
		CHECK_EQ(config.add("allowed_methods", "PUT"), 1);
		CHECK_EQ(config.add("allowed_methods", "PATCH"), 1);
		CHECK_EQ(config.add("allowed_methods", "GET POST DELETE OPTIONS"), 1);
		CHECK_EQ(config.add("allowed_methods", "POST PUT GET DELETE OPTIONS"), 1);
		CHECK_EQ(config.add("allowed_methods", "OPTIONS POST PUT GET DELETE"), 1);
	}
}

TEST_SUITE("location")
{
	TEST_CASE("location")
	{
		SUBCASE("single")
		{
			Config config;

			CHECK_EQ(config.add("location", "value"), 0);

			std::vector<location_t> locations = config.getLocations();

			CHECK_EQ(locations[0].location, "value");
		}
	}

	TEST_CASE("http_methods")
	{
		SUBCASE("default")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 3);
			CHECK_EQ(methods[0], "GET");
			CHECK_EQ(methods[1], "POST");
			CHECK_EQ(methods[2], "DELETE");
		}

		SUBCASE("GET only")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "GET"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 1);
			CHECK_EQ(methods[0], "GET");
		}

		SUBCASE("POST and GET")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "POST GET"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 2);
			CHECK_EQ(methods[0], "POST");
			CHECK_EQ(methods[1], "GET");
		}

		SUBCASE("all")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "GET POST DELETE"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 3);
			CHECK_EQ(methods[0], "GET");
			CHECK_EQ(methods[1], "POST");
			CHECK_EQ(methods[2], "DELETE");
		}

		SUBCASE("duplicated method 1")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "GET GET"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 1);
		}

		SUBCASE("duplicated method 2")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);

			std::string const testCase = "GET POST DELETE POST POST GET";
			CHECK_EQ(config.add("location_http_methods", testCase), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 3);
		}

		SUBCASE("empty")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", ""), 1);
		}

		SUBCASE("invalid http verb")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET POST DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);

			CHECK_EQ(config.add("location_http_methods", "GETS"), 1);
			CHECK_EQ(config.add("location_http_methods", "PUT"), 1);
			CHECK_EQ(config.add("location_http_methods", "GET POST OPTIONS"), 1);
			CHECK_EQ(config.add("location_http_methods", "POST GET DELET"), 1);
		}

		SUBCASE("not allowed method insertion 1")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "DELETE"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "GET"), 1);
		}

		SUBCASE("not allowed method insertion 2")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "GET POST"), 1);
		}

		SUBCASE("not allowed method insertion 3")
		{
			Config config;

			CHECK_EQ(config.add("allowed_methods", "GET"), 0);
			CHECK_EQ(config.add("location", "value"), 0);
			CHECK_EQ(config.add("location_http_methods", "GET POST"), 1);
		}
	}

	TEST_CASE("http_redirection")
	{
		Config config;

		CHECK_EQ(config.add("location", "value"), 0);

		CHECK_EQ(config.add("location_http_redirection", "301 redirect"), 0);

		std::vector<location_t> locations = config.getLocations();

		CHECK_EQ(locations[0].http_redirection.first, 301);

		CHECK_EQ(locations[0].http_redirection.second, "redirect");

		SUBCASE("bad config")
		{
			Config config;

			CHECK_EQ(config.add("location", "value"), 0);

			CHECK_EQ(config.add("location_http_redirection", "299 redirect"), 1);
			CHECK_EQ(config.add("location_http_redirection", "400 redirect"), 1);
		}

		SUBCASE("success config")
		{
			for (int i = 300; i < 400; i++)
			{
				Config config;

				CHECK_EQ(config.add("location", "value"), 0);

				std::string redirect = utils::to_string(i) + " redirect";

				CHECK_EQ(config.add("location_http_redirection", redirect), 0);
			}
		}

		SUBCASE("default")
		{
			Config config;

			CHECK_EQ(config.add("location", "value"), 0);

			location_t location = config.getLocations()[0];

			CHECK_EQ(location.http_redirection.first, 0);
		}
	}

	TEST_CASE("root")
	{
		Config config;

		CHECK_EQ(config.add("location", "value"), 0);

		CHECK_EQ(config.add("location_root", "root"), 0);

		location_t const &location = config.getLocations()[0];

		CHECK_EQ(location.root, "root");
	}

	TEST_CASE("directory_listing")
	{
		Config config;

		CHECK_EQ(config.add("location", "value"), 0);

		CHECK_EQ(config.add("location_directory_listing", "dir"), 0);

		location_t const &location = config.getLocations()[0];

		CHECK_EQ(location.directory_listing, "dir");
	}

	TEST_CASE("location_response_is_dir")
	{
		Config config;

		CHECK_EQ(config.add("location", "value"), 0);

		CHECK_EQ(config.add("location_response_is_dir", "dir"), 0);

		location_t const &location = config.getLocations()[0];

		CHECK_EQ(location.response_is_dir, "dir");
	}

	TEST_CASE("location_cgi_pass")
	{
		Config config;

		CHECK_EQ(config.add("location", "value"), 0);

		CHECK_EQ(config.add("location_cgi_pass", ".py"), 0);

		location_t const &location = config.getLocations()[0];

		CHECK_NE(location.cgi_pass.find(".py"), location.cgi_pass.end());

		SUBCASE("multiples")
		{
			Config config;

			CHECK_EQ(config.add("location", "first"), 0);
			CHECK_EQ(config.add("location_cgi_pass", ".php"), 0);

			CHECK_EQ(config.add("location", "second"), 0);
			CHECK_EQ(config.add("location_cgi_pass", ".py"), 0);

			CHECK_EQ(config.getLocations().size(), 2);

			location_t const &location_first = config.getLocations()[0];
			location_t const &location_second = config.getLocations()[1];

			CHECK_NE(location_first.cgi_pass.find(".php"), location.cgi_pass.end());
			CHECK_NE(location_second.cgi_pass.find(".py"), location.cgi_pass.end());
		}
	}

	TEST_CASE("required_cookie")
	{
		Config config;

		CHECK_EQ(config.add("location", "/"), 0);
		CHECK_EQ(config.add("location_required_cookie", "session"), 0);

		location_t const &location = config.getLocations()[0];

		CHECK_NE(location.required_cookie.find("session"),
		         location.required_cookie.end());
	}

	TEST_CASE("set_cookie")
	{
		SUBCASE("name")
		{
			Config config;

			std::string name = "test";

			CHECK_EQ(config.add("location", "/"), 0);
			CHECK_EQ(config.add("location_set_cookie", "name=\"" + name + "\""), 0);
			const t_cookie &cookie = *config.getLocations()[0].set_cookie.begin();
			CHECK_EQ(cookie.name, name);
		}

		SUBCASE("value")
		{
			Config config;

			std::string value = "test";

			CHECK_EQ(config.add("location", "/"), 0);
			CHECK_EQ(config.add("location_set_cookie", "value=\"" + value + "\""),
			         1);
			CHECK_EQ(config.add("location_set_cookie",
			                    "name=\"name\" value=\"" + value + "\""),
			         0);
			const t_cookie &cookie = *config.getLocations()[0].set_cookie.begin();
			CHECK_EQ(cookie.value, value);
			CHECK_EQ(cookie.name, "name");
		}

		SUBCASE("value")
		{
			Config config;

			std::string value = "test";

			CHECK_EQ(config.add("location", "/"), 0);
			CHECK_EQ(config.add("location_set_cookie", "value=\"" + value + "\""),
			         1);
			CHECK_EQ(config.add("location_set_cookie",
			                    "name=\"name\" value=\"" + value + "\""),
			         0);
			const t_cookie &cookie = *config.getLocations()[0].set_cookie.begin();
			CHECK_EQ(cookie.value, value);
			CHECK_EQ(cookie.name, "name");
		}

		SUBCASE("session_value")
		{
			Config config;

			CHECK_EQ(config.add("location", "/"), 0);

			CHECK_EQ(
			    config.add("location_set_cookie", "name=\"name\" session_value"), 0);

			const t_cookie &cookie = *config.getLocations()[0].set_cookie.begin();

			CHECK(cookie.sessionValue);

			SUBCASE("default")
			{
				Config config;

				CHECK_EQ(config.add("location", "/"), 0);

				CHECK_EQ(config.add("location_set_cookie", "name=\"name\" "), 0);

				const t_cookie &cookie =
				    *config.getLocations()[0].set_cookie.begin();

				CHECK_FALSE(cookie.sessionValue);
			}
		}

		SUBCASE("expires")
		{
			Config config;

			CHECK_EQ(config.add("location", "/"), 0);

			CHECK_EQ(config.add("location_set_cookie", "name=\"name\" "
			                                           "expires=\"test\""),
			         0);

			const t_cookie &cookie = *config.getLocations()[0].set_cookie.begin();

			CHECK_EQ(cookie.expires, "test");

			// TODO: not working

			// SUBCASE("error")
			// {
			// 	Config config;

			// 	CHECK_EQ(config.add("location", "/"), 0);

			// 	std::string expires = "Thu, 06 Jul 2023 07:28:00 GMT";

			// 	CHECK_EQ(config.add("location_set_cookie", "name=\"name\" "
			// 	                                           "expires=\"" +
			// 	                                               expires + "\""),
			// 	         0);

			// 	const t_cookie &cookie =
			// 	    *config.getLocations()[0].set_cookie.begin();

			// 	CHECK_EQ(cookie.expires, expires);
			// }
		}

		SUBCASE("secure")
		{
			Config config;

			CHECK_EQ(config.add("location", "/"), 0);

			CHECK_EQ(config.add("location_set_cookie", "name=\"name\" secure"), 0);

			const t_cookie &cookie = *config.getLocations()[0].set_cookie.begin();

			CHECK(cookie.secure);
		}
	}
}

TEST_SUITE("root")
{
	TEST_CASE("default")
	{
		Config config;
		CHECK_EQ(config.getMainRoot(), ".");
	}
	TEST_CASE("set and get")
	{
		Config config;
		CHECK_EQ(config.add("root", "path"), 0);
		CHECK_EQ(config.getMainRoot(), "path");
	}
}

TEST_SUITE("index")
{
	TEST_CASE("default")
	{
		Config config;
		CHECK_NE(config.getIndex().find("index.html"), config.getIndex().end());
	}
	TEST_CASE("set and get")
	{
		Config config;
		CHECK_EQ(config.add("index", "index.htm"), 0);
		CHECK_NE(config.getIndex().find("index.htm"), config.getIndex().end());
	}
	TEST_CASE("multiples")
	{
		Config      config;
		std::string index = "index.htm index.ht index.h";
		CHECK_EQ(config.add("index", index), 0);
		CHECK_NE(config.getIndex().find("index.htm"), config.getIndex().end());
		CHECK_NE(config.getIndex().find("index.ht"), config.getIndex().end());
		CHECK_NE(config.getIndex().find("index.h"), config.getIndex().end());
	}
}