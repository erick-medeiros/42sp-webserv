#include "CGIRequest.hpp"

CGIRequest::CGIRequest(void) {}

CGIRequest::~CGIRequest(void) {}

CGIRequest::CGIRequest(std::string const &resource, Connection &connection)
    : script(""), portNumber("")
{
	this->initLocation(resource, connection);
}

bool CGIRequest::isValid(void) const
{
	struct stat buff;
	return (stat(this->fileScript.c_str(), &buff) == 0);
}

void CGIRequest::exec(Request const &request, int const connectionPortNumber)
{
	std::stringstream ss;
	ss << connectionPortNumber;
	this->portNumber = ss.str();

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

	int         i = this->fileScript.find_first_of(".");
	std::string ext = std::string(fileScript.begin() + i + 1, fileScript.end());

	if (ext == "php")
		this->script = "php";
	else if (ext == "py")
		this->script = "python3";

	args.push_back(this->script);
	args.push_back(this->fileScript);
	args.push_back(r.getBody());
	this->scriptArgs = createArrayOfStrings(args);
}

void CGIRequest::initEnviromentVariables(Request const &request)
{
	std::vector<std::string> env;
	std::string              httpVersion;

	httpVersion = request.getStartLine()["version"];
	env.push_back("SERVER_PROTOCOL=" + httpVersion);

	env.push_back("SERVER_PORT=" + this->portNumber);
	env.push_back(getContentLength(request));

	env.push_back("HTTP_HOST=" + request.getHeaderValue("host"));
	env.push_back("HTTP_ACCEPT=" + request.getHeaderValue("accept"));
	env.push_back("REQUEST_METHOD=" + request.getMethod());

	std::string const resource = request.getResourcePath();
	env.push_back("PATH_INFO=" + resource);

	std::string const path = this->fileScript;
	std::string const scriptName = path.substr(path.find_last_of("/") + 1);
	env.push_back("SCRIPT_NAME=" + scriptName);

	if (request.getAllParams().size() > 0)
	{
		env.push_back("QUERY_STRING=" + request.getResourceQuery());
	}
	else
	{
		env.push_back("QUERY_STRING=");
	}

	this->envp = createArrayOfStrings(env);
}

void CGIRequest::executeCGIScript(void)
{
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		throw std::runtime_error("dup2");
	}

	std::string const bin = "/usr/bin/" + this->script;
	if (execve(bin.c_str(), this->scriptArgs, this->envp) == -1)
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

bool CGIRequest::isValidScriptLocation(std::string const &resource,
                                       Connection        &connection)
{
	std::vector<location_t> arr = connection.config.getLocations();
	for (size_t i = 0; i < arr.size(); i++)
	{
		if (arr[i].location == resource)
		{
			return true;
		}
	}
	return false;
}

void CGIRequest::initLocation(std::string const &resource, Connection &connection)
{
	std::vector<location_t> arr = connection.config.getLocations();

	for (size_t i = 0; i < arr.size(); i++)
	{
		if (arr[i].location == resource)
		{
			this->location = arr[i].location;
			this->fileScript = arr[i].cgi.path;
		}
	}
}
