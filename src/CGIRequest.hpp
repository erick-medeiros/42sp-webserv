#ifndef CGIREQUEST_HPP
#define CGIREQUEST_HPP

#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "Request.hpp"

class CGIRequest
{
  private:
	char **scriptArgs;
	char **envp;
	int    fd;

  public:
	CGIRequest(void);
	CGIRequest(std::string const &resource);
	~CGIRequest(void);

	std::string fileScript;

	bool        isValid(void) const;
	void        exec(Request const &request);
	std::string getContentLength(Request const &request) const;
	void        prepareCGIRequest(Request const &request);
	void        initTemporaryDescriptor(Request const &request);
	void        initScriptArguments(Request const &request);
	void        initEnviromentVariables(Request const &request);
	void        executeCGIScript(void);
	char      **createArrayOfStrings(std::vector<std::string> const &envVars) const;
	void        destroyArrayOfStrings(char **envp) const;
};

#endif