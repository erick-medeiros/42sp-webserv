#include "Server.hpp"
#include "doctest.h"
#include <sstream>
#include <streambuf>

TEST_CASE("Server")
{
	// TODO -> Improve this test when we have the config files
	WHEN("Has a specific port number")
	{
		freopen("/dev/null", "w", stdout);
		Config config;
		config.add("port", "8080");

		Server server;
		server.init(config);
		freopen("/dev/tty", "w", stdout);
		THEN("The port number is set")
		{
			CHECK(server.getPort() == 8080);
		}
	}
}