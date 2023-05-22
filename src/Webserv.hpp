// BEGIN: abpxx6d04wxr
#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

class Webserv
{
public:
    Webserv(int port);
    ~Webserv();
    void run();

private:
    int server_socket_fd;
    sockaddr_in createServerAddress(int port);
};

#endif
