#ifndef CGIREQUEST_HPP
#define CGIREQUEST_HPP

#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "Connection.hpp"
#include "Request.hpp"

class Connection;

class CGIRequest
{
  private:
	char **scriptArgs;
	char **envp;
	int    fd;

	Connection const &_connection;

	std::string fileScript;
	std::string script;
	std::string portNumber;
	std::string getContentLength() const;
	void        prepareCGIRequest();
	void        initTemporaryDescriptor();
	void        initScriptArguments();
	void        initEnviromentVariables();
	void        executeCGIScript(void);
	char      **createArrayOfStrings(std::vector<std::string> const &envVars) const;
	void        destroyArrayOfStrings(char **envp) const;

  public:
	CGIRequest(std::string const &resource, Connection &connection);
	~CGIRequest(void);

	int exec(void);
};

#endif
