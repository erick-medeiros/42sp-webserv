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
	std::map<int, Connection> connections;
	std::map<int, Server>     serversMap;
	Server                    servers[configs.size()]; // Array of servers
	EpollWrapper              epoll(MAX_EVENTS * configs.size());
	Cookie                    cookies;

	size_t i = 0;
	while (i < configs.size())
	{
		Server &server = servers[i];
		Config &config = configs[i];

		server.init(config);

		if (server.getServerSocket() < 0)
		{
			log.error("Error creating server");
			running(false);
			return 1;
		}
		epoll.add(server.getServerSocket());
		serversMap.insert(std::pair<int, Server>(server.getServerSocket(), server));
		i++;
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
				Server *server = &serversMap[fd];
				Connection *cnn = new Connection(*server);
				connections.insert(std::pair<int, Connection>(cnn->fd, *cnn));
				epoll.add(cnn->fd);
				continue;
			}

			// Existing connection
			Connection *connection = &connections.find(fd)->second;

			if (event.events & EPOLLERR)
			{
				removeConnection(connection, epoll);
				continue;
			}

			if (event.events & (EPOLLRDHUP | EPOLLHUP))
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
						epoll.modify(connection->fd);
					}
				}
				catch (std::exception const &e)
				{
					log.error(e.what());
					removeConnection(connection, epoll);
				}
				continue;
			}

			// Response
			if (event.events & EPOLLOUT)
			{
				connection->server.handleRequest(*connection);
				connection->sendHttpResponse();
				removeConnection(connection, epoll);
				continue;
			}
		}
	}
	return 0;
}
