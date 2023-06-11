#include "CGIRequest.hpp"

CGIRequest::CGIRequest(void) {}

CGIRequest::~CGIRequest(void) {}

CGIRequest::CGIRequest(std::string const &resource)
{
	this->fileScript = "public" + resource;
}

bool CGIRequest::isValid(void) const
{
	struct stat buff;
	return (stat(this->fileScript.c_str(), &buff) == 0);
}

void CGIRequest::exec(Request const &request)
{
	prepareCGIRequest(request);
	executeCGIScript();
}

void CGIRequest::prepareCGIRequest(Request const &request)
{
	initTemporaryDescriptor(request);
	initScriptArguments(request);
	initEnviromentVariables(request);
}

void CGIRequest::initTemporaryDescriptor(Request const &r)
{
	std::stringstream ss;
	ss << r.getFd();
	std::string const tempFile(CGI_RESPONSE + ss.str());
	this->fd = open(tempFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
}

void CGIRequest::initScriptArguments(Request const &r)
{
	std::vector<std::string> args;
	args.push_back("php");
	args.push_back(this->fileScript);
	args.push_back(r.getBody());
	this->scriptArgs = createArrayOfStrings(args);
}

void CGIRequest::initEnviromentVariables(Request const &request)
{
	std::vector<std::string> env;
	env.push_back("SERVER_PORT=8080");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back(getContentLength(request));

	env.push_back("HTTP_HOST=" + request.getHeaderValue("host"));
	env.push_back("HTTP_ACCEPT=" + request.getHeaderValue("accept"));
	env.push_back("REQUEST_METHOD=" + request.getMethod());

	std::string const resource = request.getResourcePath();
	std::string const scriptName = std::string(resource.begin() + 1, resource.end());
	env.push_back("PATH_INFO=" + resource);
	env.push_back("SCRIPT_NAME=" + scriptName);

	if (request.getAllParams().size() > 0)
	{
		env.push_back("QUERY_STRING=" + request.getResourceQuery());
	}

	this->envp = createArrayOfStrings(env);
}

void CGIRequest::executeCGIScript(void)
{
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		throw std::runtime_error("dup2");
	}

	if (execve("/bin/php", this->scriptArgs, this->envp) == -1)
	{
		throw std::runtime_error("execve");
	}
}

std::string CGIRequest::getContentLength(Request const &r) const
{
	if (r.getBody().size() > 0)
	{
		std::string len = r.getHeaderValue("content-length");
		return "CONTENT_LENGTH=" + len;
	}
	return "CONTENT_LENGTH=0";
}

char **
CGIRequest::createArrayOfStrings(std::vector<std::string> const &envVars) const
{
	char **envp = new char *[envVars.size() + 1];

	for (std::size_t i = 0; i < envVars.size(); ++i)
	{
		envp[i] = new char[envVars[i].size() + 1];
		std::strcpy(envp[i], envVars[i].c_str());
	}
	envp[envVars.size()] = NULL;

	return envp;
}

void CGIRequest::destroyArrayOfStrings(char **envp) const
{
	for (char **p = envp; *p; ++p)
	{
		delete[] * p;
	}
	delete[] envp;
}