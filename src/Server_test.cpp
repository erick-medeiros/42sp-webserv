#include "Server.hpp"
#include "doctest.h"
#include <sstream>
#include <streambuf>

TEST_CASE("Server")
{
	// redirect std::cout
	std::streambuf *   oldCoutStreamBuf = std::cout.rdbuf();
	std::ostringstream oss;
	std::cout.rdbuf(oss.rdbuf());

	// TODO -> Improve this test when we have the config files
	WHEN("Has a specific port number")
	{
		Server server("config_file");
		THEN("The port number is set")
		{
			CHECK(server.getPort() == 8080);
		}
	}

	// restore old std::cout
	std::cout.rdbuf(oldCoutStreamBuf);
}