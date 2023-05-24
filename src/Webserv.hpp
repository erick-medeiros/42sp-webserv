#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <cstring>      // memset
#include <fstream>      // input stream
#include <iostream>     // cout, cerr
#include <iostream>     // input stream
#include <netinet/in.h> // sockaddr_in
#include <sstream>      // stringstream
#include <stdlib.h>     // exit
#include <string>       // to_string
#include <unistd.h>     // close

class Webserv
{
  public:
	Webserv(int port);
	~Webserv();
	void run();

  private:
	int         server_socket_fd;
	sockaddr_in createServerAddress(int port);
};

#endif
