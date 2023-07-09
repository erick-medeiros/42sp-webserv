#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdlib>  // exit
#include <cstring>  // memset
#include <fcntl.h>  // fcntl
#include <fstream>  // input stream
#include <iostream> // cout, cerr
#include <map>
#include <netinet/in.h> // sockaddr_in
#include <sstream>      // stringstream
#include <stdexcept>
#include <string>     // to_string
#include <sys/stat.h> // mkdir
#include <sys/wait.h>
#include <unistd.h> // close
#include <vector>

#include "CGIRequest.hpp"
#include "Config.hpp"
#include "Connection.hpp"
#include "Cookie.hpp"
#include "EpollWrapper.hpp"
#include "HttpStatus.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define MAX_EVENTS 500

class Connection;

class Server
{
  public:
	Server(void);
	~Server(void);
	void init(Config const &conf);
	int  getPort(void);

	int     getServerSocket();
	Config &getConfig(void);

	static std::string getRequestData(int clientSocket);
	void               handleRequest(Connection &connection);

  private:
	Config _config;
	int    _serverSocket;

	int         listenToPort(int port);
	void        handleMultipart(Connection &connection);
	void        handleCookies(Connection &, std::vector<location_t> &);
	sockaddr_in createServerAddress(int port);
};

#endif
