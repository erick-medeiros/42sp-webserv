/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/29 11:40:39 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Logger.hpp"
#include "doctest.h"
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
		SUBCASE("GET")
		{
			Config config;

			CHECK_EQ(config.add("location", "value"), 0);

			CHECK_EQ(config.add("location_http_methods", "GET"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods.size(), 1);
			CHECK_EQ(methods[0], "GET");
		}
		SUBCASE("all")
		{
			Config config;

			CHECK_EQ(config.add("location", "value"), 0);

			CHECK_EQ(config.add("location_http_methods", "GET POST DELETE"), 0);

			std::vector<location_t>   locations = config.getLocations();
			std::vector<std::string> &methods = locations[0].http_methods;

			CHECK_EQ(methods[0], "GET");
			CHECK_EQ(methods[1], "POST");
			CHECK_EQ(methods[2], "DELETE");
			CHECK_EQ(methods.size(), 3);
		}
	}

	TEST_CASE("http_redirection")
	{
		Config config;

		CHECK_EQ(config.add("location", "value"), 0);

		CHECK_EQ(config.add("location_http_redirection", "redirect"), 0);

		std::vector<location_t> locations = config.getLocations();

		CHECK_EQ(locations[0].http_redirection, "redirect");
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

		CHECK_EQ(config.add("location_cgi_pass", "/bin"), 0);

		location_t const &location = config.getLocations()[0];

		CHECK_EQ(location.cgi_pass, "/bin");

		SUBCASE("error")
		{
			Config config;

			CHECK_EQ(config.add("location", "value"), 0);

			CHECK_EQ(config.add("location_cgi_pass", "/bin extra"), 1);
		}

		SUBCASE("multiples")
		{
			Config config;

			CHECK_EQ(config.add("location", "first"), 0);
			CHECK_EQ(config.add("location_cgi_pass", "/php"), 0);

			CHECK_EQ(config.add("location", "second"), 0);
			CHECK_EQ(config.add("location_cgi_pass", "/python"), 0);

			CHECK_EQ(config.getLocations().size(), 2);

			location_t const &location_first = config.getLocations()[0];
			location_t const &location_second = config.getLocations()[1];

			CHECK_EQ(location_first.cgi_pass, "/php");
			CHECK_EQ(location_second.cgi_pass, "/python");
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