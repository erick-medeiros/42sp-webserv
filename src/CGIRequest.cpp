#include "CGIRequest.hpp"

CGIRequest::~CGIRequest(void)
{
	std::remove(getFileName().c_str());
}

CGIRequest::CGIRequest(std::string const &resource, Connection &connection)
    : _connection(connection), script(""), portNumber("")
{
	this->fileScript = resource;
	_fileName = CGI_RESPONSE + utils::to_string(connection.fd);
}

std::string CGIRequest::exec(void)
{
	if (access(this->fileScript.c_str(), R_OK) != 0)
	{
		log.error("CGI exec: invalid file", strerror(errno));
		return "";
	}

	std::signal(SIGCHLD, SIG_IGN);

	int pid = fork();
	if (pid == 0)
	{
		std::stringstream ss;
		ss << _connection.config.getPort();
		this->portNumber = ss.str();

		prepareCGIRequest();
		executeCGIScript();

		return "";
	}

	int max_try = 3;
	int status;

	while (max_try > 0)
	{
		log.debug("CGI: try: " + utils::to_string(max_try));

		sleep(1);

		waitpid(pid, &status, WNOHANG);

		if (status & WNOHANG)
			max_try--;
		else
			max_try = 0;
	}

	if (status & WNOHANG)
	{
		kill(pid, SIGKILL);
	}

	waitpid(pid, NULL, 0);

	return utils::readFile(getFileName());
}

std::string const &CGIRequest::getFileName(void) const
{
	return _fileName;
}

void CGIRequest::prepareCGIRequest()
{
	initTemporaryDescriptor();
	initScriptArguments();
	initEnviromentVariables();
}

void CGIRequest::initTemporaryDescriptor()
{
	std::stringstream ss;
	ss << _connection.fd;
	std::string const tempFile(CGI_RESPONSE + ss.str());
	this->fd = open(tempFile.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
}

void CGIRequest::initScriptArguments()
{
	std::vector<std::string> args;

	int         i = this->fileScript.find_last_of(".");
	std::string ext = std::string(fileScript.begin() + i + 1, fileScript.end());

	if (ext == "php")
		this->script = "php";
	else if (ext == "py")
		this->script = "python3";

	args.push_back(this->script);
	args.push_back(this->fileScript);
	args.push_back(_connection.request.getBody());
	this->scriptArgs = createArrayOfStrings(args);
}

void CGIRequest::initEnviromentVariables()
{
	std::vector<std::string> env;
	std::string              httpVersion;

	Request const &request = _connection.request;

	httpVersion = request.getStartLine()["version"];
	env.push_back("SERVER_PROTOCOL=" + httpVersion);

	env.push_back("SERVER_PORT=" + this->portNumber);
	env.push_back(getContentLength());

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

std::string CGIRequest::getContentLength() const
{
	Request const &r = _connection.request;
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
