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
#include <sys/wait.h>
#include <unistd.h> // close
#include <vector>

#include "CGIRequest.hpp"
#include "Config.hpp"
#include "Cookie.hpp"
#include "EpollWrapper.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "log_utils.hpp"
#include "signals.hpp"

#define MAX_EVENTS 500
#define BLOCK_IND -1

class Server
{
  public:
	Server(void);
	~Server(void);
	void init(Config const &conf);
	void run(void);
	int  getPort(void);

	int      getServerSocket();
	Request *getRequestSocket(void);

	int                acceptNewClient(void);
	static int         disconnectClient(Request *request);
	static std::string getRequestData(Request *request);
	static int         requestClient(Request *request, EpollWrapper &epoll);
	static int responseClient(Request *request, EpollWrapper &epoll, Cookie &cookies,
	                          Config &config);

  private:
	Request     *_requestSocket;
	Config       _config;
	int          serverSocket;
	EpollWrapper monitoredSockets;
	int          listenToPort(int port);
	sockaddr_in  createServerAddress(int port);
};

#endif
