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

void removeConnection(channel_t *channel, EpollWrapper &epoll)
{
	if (!channel)
		return;
	Connection *connection = reinterpret_cast<Connection *>(channel->ptr);
	epoll.remove(connection->fd);
	connection->disconnect();
	delete connection;
	delete channel;
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
	Server       servers[configs.size()];
	EpollWrapper epoll(MAX_EVENTS * configs.size());
	Cookie       cookies;
	channel_t    channelServers[configs.size()];

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

		{ // socket
			channel_t *channel = &channelServers[i];
			channel->type = channel_t::CHANNEL_SOCKET;
			channel->ptr = &servers[i];
			epoll_data_t data = {channel};
			epoll.add(server.getServerSocket(), data, EPOLLIN | EPOLLOUT);
		}

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
			channel_t *channel = reinterpret_cast<channel_t *>(event.data.ptr);

			if (channel->type == channel_t::CHANNEL_SOCKET)
			{
				Server *server = reinterpret_cast<Server *>(channel->ptr);
				{ // client
					Connection *cnn = new Connection(*server);
					if (cnn->fd < 0)
					{
						delete cnn;
						continue;
					}
					channel_t *channelConnection = new channel_t;
					channelConnection->type = channel_t::CHANNEL_CONNECTION;
					channelConnection->ptr = cnn;
					epoll_data_t data = {channelConnection};
					epoll.add(cnn->fd, data, EPOLLIN);
				}
				continue;
			}

			Connection *connection = reinterpret_cast<Connection *>(channel->ptr);

			if (event.events & EPOLLERR)
			{
				log.error("Epoll error on socket", connection->fd);
				removeConnection(channel, epoll);
				continue;
			}

			if (event.events & (EPOLLRDHUP | EPOLLHUP))
			{
				removeConnection(channel, epoll);
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
						epoll_data_t data = {channel};
						epoll.modify(connection->fd, data, EPOLLOUT);
					}
				}
				catch (std::exception const &e)
				{
					log.error(e.what());
					removeConnection(channel, epoll);
				}
				continue;
			}

			// Response
			if (event.events & EPOLLOUT)
			{
				connection->server.handleRequest(*connection);
				connection->sendHttpResponse();
				removeConnection(channel, epoll);
				continue;
			}
		}
	}
	return 0;
}
