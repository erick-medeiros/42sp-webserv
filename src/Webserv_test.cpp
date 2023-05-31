#include "Webserv.hpp"
#include "doctest.h"
#include <sstream>
#include <streambuf>

TEST_CASE("Webserver")
{
	// redirect std::cout
	std::streambuf *   oldCoutStreamBuf = std::cout.rdbuf();
	std::ostringstream oss;
	std::cout.rdbuf(oss.rdbuf());

	WHEN("Has a specific port number")
	{
		Webserv webserver(1337);
		THEN("The port number is set")
		{
			CHECK(webserver.getPort() == 1337);
		}
	}
	WHEN("Has no port number")
	{
		Webserv webserver(0);
		THEN("It uses a random port")
		{
			CHECK(webserver.getPort() != 0);
		}
	}

	// restore old std::cout
	std::cout.rdbuf(oldCoutStreamBuf);
}