#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>      // memset
#include <fstream>      // input stream
#include <iostream>     // cout, cerr
#include <iostream>     // input stream
#include <netinet/in.h> // sockaddr_in
#include <sstream>      // stringstream
#include <stdlib.h>     // exit
#include <string>       // to_string
#include <unistd.h>     // close

#include "Request.hpp"
#include "Response.hpp"
#include "log_utils.hpp"

class Request;

class Server
{
  public:
	Server(std::string config_file);
	~Server();
	void run();
	int  getPort();

  private:
	int         serverSocket;
	int         listenToPort(int port);
	sockaddr_in createServerAddress(int port);
};

#endif
