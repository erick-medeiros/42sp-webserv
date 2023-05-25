#include "Webserv.hpp"
#include "doctest.h"

TEST_CASE("Webserver")
{
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
}