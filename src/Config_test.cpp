/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/10 17:55:23 by eandre-f         ###   ########.fr       */
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
