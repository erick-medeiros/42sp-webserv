#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>      // memset
#include <fcntl.h>      // fcntl
#include <fstream>      // input stream
#include <iostream>     // cout, cerr
#include <iostream>     // input stream
#include <netinet/in.h> // sockaddr_in
#include <sstream>      // stringstream
#include <stdlib.h>     // exit
#include <string>       // to_string
#include <unistd.h>     // close

#include "EpollWrapper.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "log_utils.hpp"

#define MAX_EVENTS 500

class Request;

class Server
{
  public:
	Server(std::string config_file);
	~Server();
	void run();
	int  getPort();

  private:
	int          serverSocket;
	int          listenToPort(int port);
	sockaddr_in  createServerAddress(int port);
	int          acceptNewClient(void);
	std::string  getRawRequest(int clientSocket);
	EpollWrapper waiting_list;
};

#endif
