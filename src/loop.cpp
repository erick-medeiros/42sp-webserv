/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mi <mi@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 10:55:41 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/08 12:58:11 by mi               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Connection.hpp"
#include "EpollWrapper.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <csignal>

bool running(bool status)
{
	static bool mode = true;
	if (!status)
		mode = false;
	return mode;
}

void shutdown(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT)
		running(false);
}

void removeConnection(Connection *connection, EpollWrapper &epoll)
{
	epoll.remove(connection->fd);
	connection->disconnect();
	delete connection;
}

int loop(std::string path_config)
{
	std::signal(SIGINT, shutdown);
	std::signal(SIGQUIT, shutdown);

	std::string         file;
	std::vector<Config> configs;
	try
	{
		file = Config::readFile(path_config);
		configs = Config::parseConfig(file);
	}
	catch (...)
	{
		return (1);
	}
	std::map<int, Connection*> connections;
	std::map<int, Server*>     servers;
	EpollWrapper               epoll(MAX_EVENTS * configs.size());
	Cookie                     cookies;

	// Setup servers
	for (size_t i = 0; i < configs.size(); i++)
	{
		Server *server = new Server();
		Config &config = configs[i];

		server->init(config);

		if (server->getServerSocket() < 0)
		{
			log.error("Error creating server");
			running(false);
			return 1;
		}
		epoll.add(server->getServerSocket());
		servers[server->getServerSocket()] = server;
	}

	while (running(true))
	{
		int numEvents = epoll.wait(0);

		if (numEvents == -1)
			running(false);

		for (int i = 0; i < numEvents; ++i)
		{
			struct epoll_event &event = epoll.events[i];

			int fd = event.data.fd;

			// New connection
			if (connections.find(fd) == connections.end())
			{
				Server *server = servers[fd];
				// Create a connection
				Connection *connection = new Connection(*server);
				connections[connection->fd] = connection;
				epoll.add(connection->fd);
				continue;
			}

			// Existing connection
			Connection *connection = connections[fd];

			if (event.events & EPOLLERR || event.events & (EPOLLRDHUP | EPOLLHUP))
			{
				removeConnection(connection, epoll);
				continue;
			}

			// Request
			if (event.events & EPOLLIN)
			{
				try
				{
					std::string rawRequest = Server::getRequestData(connection->fd);
					connection->request.parse(rawRequest, connection->config);
					if (connection->request.isParsed())
					{
						if (Logger::level == LOGGER_LEVEL_DEBUG)
							std::cout << connection->request << std::endl;
						connection->server.handleRequest(*connection);
						connection->sendHttpResponse();
						removeConnection(connection, epoll);
						continue;
					}
				}
				catch (std::exception const &e)
				{
					log.error(e.what());
					removeConnection(connection, epoll);
				}
				continue;
			}
		}
	}

	// Cleanup servers
	std::map<int, Server*>::iterator it;
	for (it = servers.begin(); it != servers.end(); it++)
		delete it->second;
	return 0;
}
