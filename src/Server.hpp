#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>  // memset
#include <fcntl.h>  // fcntl
#include <fstream>  // input stream
#include <iostream> // cout, cerr
#include <map>
#include <netinet/in.h> // sockaddr_in
#include <sstream>      // stringstream
#include <stdexcept>
#include <stdlib.h> // exit
#include <string>   // to_string
#include <unistd.h> // close
#include <vector>

#include "EpollWrapper.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "log_utils.hpp"
#include "signals.hpp"

#define MAX_EVENTS 500

class Server
{
  public:
	Server(void);
	~Server(void);
	void init(int argc, char **argv);
	void run(void);
	int  getPort(void);

  private:
	int          serverSocket;
	EpollWrapper waiting_list;
	int          listenToPort(int port);
	sockaddr_in  createServerAddress(int port);
	int          acceptNewClient(void);
	std::string  getRawRequest(int clientSocket);
	int          disconnectClient(int clientSocket);
};

#endif
