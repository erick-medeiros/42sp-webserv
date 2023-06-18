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

	void initLocation(std::string const &resource, Connection &connection);

  public:
	CGIRequest(void);
	CGIRequest(std::string const &resource, Connection &connection);
	~CGIRequest(void);

	std::string fileScript;
	std::string script;
	std::string portNumber;
	std::string location;

	bool        isValid(void) const;
	void        exec(Request const &request, int const connectionPortNumber);
	std::string getContentLength(Request const &request) const;
	void        prepareCGIRequest(Request const &request);
	void        initTemporaryDescriptor(Request const &request);
	void        initScriptArguments(Request const &request);
	void        initEnviromentVariables(Request const &request);
	void        executeCGIScript(void);
	char      **createArrayOfStrings(std::vector<std::string> const &envVars) const;
	void        destroyArrayOfStrings(char **envp) const;
	static bool isValidScriptLocation(std::string const &resource,
	                                  Connection        &connection);
};

#endif
