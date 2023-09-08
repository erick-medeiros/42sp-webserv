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
#include "EventWrapper.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <csignal>
#include <typeinfo> // Used to get the name of the event wrapper

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

void removeConnection(Connection *connection, EventWrapper *eventWrapper)
{
	eventWrapper->remove(connection->fd);
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
		if (configs.empty())
			throw std::runtime_error("No server found in config file");
	}
	catch (...)
	{
		return (1);
	}

	std::cout << "Config file: " << path_config << std::endl;

	std::map<int, Connection *> connections;
	std::map<int, Server *>     servers;
	Cookie                      cookies;
	EventWrapper               *eventWrapper = createEventWrapper();

	// Setup servers
	for (size_t i = 0; i < configs.size(); i++)
	{
		Server *server = new Server();
		Config &config = configs[i];

		server->init(config);

		if (server->getServerSocket() < 0)
		{
			logger.error("Error creating server");
			running(false);
			return 1;
		}
		eventWrapper->add(server->getServerSocket());
		servers[server->getServerSocket()] = server;
	}

	while (running(true))
	{
		std::vector<Event> events = eventWrapper->getEvents(1000);

		for (std::vector<Event>::const_iterator it = events.begin();
		     it != events.end(); ++it)
		{
			int fd = it->fd;

			// New connection
			if (connections.find(fd) == connections.end())
			{
				Server     *server = servers[fd];
				Connection *connection = new Connection(*server);
				connections[connection->fd] = connection;
				eventWrapper->add(connection->fd);
				continue;
			}

			// Existing connection
			Connection *connection = connections[fd];
			if (it->type == ERROR_EVENT || it->type == CLOSE_EVENT)
			{
				removeConnection(connection, eventWrapper);
				continue;
			}

			if (it->type == READ_EVENT)
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
						removeConnection(connection, eventWrapper);
					}
				}
				catch (std::exception const &e)
				{
					logger.error(e.what());
					removeConnection(connection, eventWrapper);
				}
			}
		}
	}

	// Cleanup servers
	std::map<int, Server *>::iterator it;
	for (it = servers.begin(); it != servers.end(); it++)
		delete it->second;
	delete eventWrapper;
	return 0;
}