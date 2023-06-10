/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/10 19:18:53 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "doctest.h"
#include <sys/types.h>

TEST_SUITE("Class Config")
{
	TEST_CASE("Constructor")
	{
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

		vector<int> const &ports = config.getPorts();

		CHECK_EQ(ports.size(), 1);
		CHECK_EQ(ports[0], 8080);
	}
	TEST_CASE("multiple ports")
	{
		Config config;
		CHECK_EQ(config.add("port", "8080 8090 9000"), 0);
		vector<int> const &ports = config.getPorts();
		CHECK_EQ(ports.size(), 3);
		CHECK_EQ(ports[0], 8080);
		CHECK_EQ(ports[1], 8090);
		CHECK_EQ(ports[2], 9000);
	}
	TEST_CASE("limits")
	{
		Config config;
		freopen("/dev/null", "w", stderr);
		CHECK_EQ(config.add("port", "1023"), 1);
		CHECK_EQ(config.add("port", "49152"), 1);
		freopen("/dev/tty", "w", stderr);
		vector<int> const &ports = config.getPorts();
		CHECK_EQ(ports.size(), 0);
		CHECK_EQ(config.add("port", "1024 49151"), 0);
		CHECK_EQ(ports.size(), 2);
	}
}

TEST_SUITE("server name")
{
	TEST_CASE("single")
	{
		Config config;
		string server = "localhost";
		CHECK_EQ(config.add("server_name", server), 0);
		vector<string> const &serverNames = config.getServerNames();

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
		Config config;
		string server1 = "localhost";
		string server2 = "www.localhost";

		CHECK_EQ(config.add("server_name", server1 + " " + server2), 0);

		{ // error
			freopen("/dev/null", "w", stderr);
			CHECK_EQ(config.add("server_name", server2), 1);
			freopen("/dev/tty", "w", stderr);
		}

		vector<string> const &serverNames = config.getServerNames();

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
		// TODO: make
	}
}
