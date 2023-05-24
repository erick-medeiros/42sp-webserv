#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream> // cout, cerr
#include <cstring> // memset
#include <stdlib.h> // exit
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> // close
#include <iostream> // input stream
#include <fstream> // input stream
#include <sstream> // stringstream
#include <string> // to_string

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
