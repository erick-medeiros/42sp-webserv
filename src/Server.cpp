#include "Server.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char **createEnvp(std::vector<std::string> const &envVariables);

bool isValidCgiScript(std::string const &filename)
{
	struct stat buff;
	return (stat(filename.c_str(), &buff) == 0);
}

Server::Server(void) : monitoredSockets(MAX_EVENTS) {}

void Server::init(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	logSuccess("initializing new web server");

	initSignal(this);

	listenToPort(8080);
}

Server::~Server()
{
	close(serverSocket);
}

int Server::listenToPort(int port)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		logError("--- Error: socket");
		exit(1);
	}
	logInfo("Server socket created", serverSocket);
	// --- Configure socket options ---
	int yes = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		logError("--- Error: setsockopt");
		exit(1);
	}
	// --- Bind socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(serverSocket, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		logError("--- Error: binding");
		close(serverSocket);
		exit(1);
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(serverSocket))
	{
		logError("--- Error: fcntl");
		exit(1);
	}
	// --- Set socket to listen for connections ---
	if (listen(serverSocket, 5) < 0)
	{
		logError("--- Error: listen");
		exit(1);
	}
	logInfo("Server listening on port", port);
	return serverSocket;
}

int Server::getPort()
{
	sockaddr_in address;
	socklen_t   len = sizeof(address);
	if (getsockname(serverSocket, (struct sockaddr *) &address, &len) < 0)
	{
		logError("--- Error: getsockname");
		exit(1);
	}
	return ntohs(address.sin_port);
}

int Server::acceptNewClient(void)
{
	sockaddr_in clientAddr;
	socklen_t   clilen = sizeof(clientAddr);

	int clientSocket =
	    accept(serverSocket, (struct sockaddr *) &clientAddr, &clilen);
	if (clientSocket == -1)
	{
		logError("--- Error: accept");
		exit(1);
	}
	logSuccess("+++ New connection accepted on socket", clientSocket);
	return clientSocket;
}

std::string Server::readFromSocket(epoll_event *event)
{
	Request *request = reinterpret_cast<Request *>(event->data.ptr);
	int      clientSocket = request->getFd();

	int  buffSize = 1024;
	char buff[buffSize];

	int bytesRead = recv(clientSocket, buff, sizeof(buff), 0);

	if (bytesRead == -1)
	{
		logError("--- Error: while receiving data");
		return "";
	}
	if (bytesRead == 0)
	{
		logError("--- Error: client has closed its connection");
		close(clientSocket);
		return "";
	}
	// TODO: Não funciona com o caso de um request maior que 1024 bytes, pois
	//       não é feito um loop para ler o resto do request e não aciona outro
	//       EPOLLIN
	//		 ideia: Fazer o Request saber quando está completo e se não estiver fazer
	// outra chamada
	if (bytesRead < buffSize)
		monitoredSockets.modify(clientSocket, event->data, EPOLLOUT);
	buff[bytesRead] = 0;
	logInfo("Request size", bytesRead);
	return std::string(buff);
}

int Server::disconnectClient(Request *request)
{
	if (!request)
		return 0;
	int fd = request->getFd();
	logInfo("--- Client disconnected from socket", fd);
	monitoredSockets.remove(fd);
	delete request;
	return close(fd);
}

#ifndef FEATURE_FLAG_COOKIE
#define FEATURE_FLAG_COOKIE 0
#endif

void Server::run()
{
	// --- Add server socket to waiting list, so it is managed by epoll ---
	// TODO: remove request of socket
	Request     *request_socket = new Request(serverSocket);
	epoll_data_t data = {0};
	data.ptr = request_socket;
	monitoredSockets.add(serverSocket, data, EPOLLIN | EPOLLOUT);

	Cookie cookies;

	while (true)
	{
		int numEvents = monitoredSockets.wait(BLOCK_IND);

		for (int i = 0; i < numEvents; ++i)
		{
			struct epoll_event &event = monitoredSockets.events[i];
			Request *request = reinterpret_cast<Request *>(event.data.ptr);

			// New connection on server
			if (request->getFd() == serverSocket)
			{
				int newClient = acceptNewClient();
				setNonBlocking(newClient);
				epoll_data_t data;
				data.ptr = new Request(newClient);
				monitoredSockets.add(newClient, data, EPOLLIN);
				continue;
			}

			if (event.events & EPOLLERR)
			{
				logError("Epoll error on socket", request->getFd());
				disconnectClient(request);
				continue;
			}

			if (event.events & (EPOLLRDHUP | EPOLLHUP))
			{
				logError("Client disconnected from socket", request->getFd());
				disconnectClient(request);
				continue;
			}

			// Request
			if (event.events & EPOLLIN)
			{
				try
				{
					std::string rawInput = readFromSocket(&event);
					request->parse(rawInput);

					std::cout << *request << std::endl;

					// Handle CGI
					std::string resource = request->getResourcePath();
					resource = trim(resource);

					if (resource.find_last_of(".php") != std::string::npos)
					{
						std::string const fileScript("public" + resource);
						if (isValidCgiScript(fileScript))
						{
							request->setCgiAs(true);
							int status;
							int pid = fork();
							if (pid == 0)
							{
								std::stringstream ss;
								ss << request->getFd();
								std::string const tempFile(".temp-" + ss.str());
								int               fd = open(tempFile.c_str(),
								                            O_CREAT | O_RDWR | O_TRUNC, 0644);

								std::vector<std::string> args;
								args.push_back("php");
								args.push_back(fileScript);
								args.push_back(request->getBody());
								char **scriptArgs = createEnvp(args);

								std::vector<std::string> env;
								env.push_back("SERVER_PORT=8080");
								env.push_back("SERVER_PROTOCOL=HTTP/1.1");
								if (request->getBody().size() > 0)
								{
									std::string contentLength;
									contentLength = request->getHeaderValue("Content"
									                                        "-"
									                                        "Lengt"
									                                        "h");
									if (contentLength.empty())
									{
										contentLength = request->getHeaderValue("con"
										                                        "ten"
										                                        "t"
										                                        "-le"
										                                        "ngt"
										                                        "h");
									}
									env.push_back(std::string("CONTENT_LENGTH=") +
									              contentLength);
								}
								else
								{
									env.push_back("CONTENT_LENGTH=0");
								}
								env.push_back(std::string("HTTP_HOST=") +
								              request->getHeaderValue("Host"));
								env.push_back(std::string("HTTP_ACCEPT=") +
								              request->getHeaderValue("Accept"));
								env.push_back(std::string("REQUEST_METHOD=") +
								              request->getMethod());
								env.push_back(std::string("PATH_INFO=") +
								              request->getResourcePath());
								env.push_back(std::string("SCRIPT_NAME=") +
								              std::string(resource.begin() + 1,
								                          resource.end()));

								if (request->getAllParams().size() > 0)
								{
									env.push_back(std::string("QUERY_STRING=") +
									              request->getResourceQuery());
								}

								char **envp = createEnvp(env);

								if (dup2(fd, STDOUT_FILENO) == -1)
								{
									throw std::runtime_error("dup2");
								}

								execve("/bin/php", scriptArgs, envp);
							}
							waitpid(pid, &status, 0);
						}
					}
					else
					{
						request->setCgiAs(false);
					}
				}
				catch (std::exception const &e)
				{
					// TODO: handle exception properly...
					logError(e.what());
					disconnectClient(request);
				}
				continue;
			}

			// Response
			if (event.events & EPOLLOUT)
			{
				Response response(*request);

				if (FEATURE_FLAG_COOKIE) // test
				{
					string username = Cookie::getUsername(*request);
					if (username == "")
					{
						string value = Cookie::getValueCookie(*request, "session");
						if (cookies.get(value) != "")
						{
							response.setStatus(200);
							response.setBody("username " + cookies.get(value));
						}
					}
					else
					{
						string session = cookies.generateSession();
						cookies.set(session, username);
						response.setHeader("Set-Cookie",
						                   "session=" + session + ";path=/");
					}
				}

				response.sendHttpResponse();
				disconnectClient(request);
			}
		}
	}

	delete request_socket;
	monitoredSockets.remove(serverSocket);
}

// --- Helper functions ---
sockaddr_in Server::createServerAddress(int port)
{
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	return address;
}

char **createEnvp(std::vector<std::string> const &envVariables)
{
	char **envp = new char *[envVariables.size() + 1];

	for (std::size_t i = 0; i < envVariables.size(); ++i)
	{
		envp[i] = new char[envVariables[i].size() + 1];
		std::strcpy(envp[i], envVariables[i].c_str());
	}
	envp[envVariables.size()] = NULL;

	return envp;
}

void destroyEnvp(char **envp)
{
	for (char **p = envp; *p; ++p)
	{
		delete[] * p;
	}
	delete[] envp;
}
