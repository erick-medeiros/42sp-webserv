/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 16:04:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/16 08:53:13 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "EpollWrapper.hpp"
#include "Server.hpp"

#define DEFAULT_CONF "./config/default.conf"

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

int main(int argc, char *argv[])
{
	std::signal(SIGINT, shutdown);
	std::signal(SIGQUIT, shutdown);

	if (argc > 1 &&
	    std::string(argv[1]) == "NORUN") // TODO: Improve this, used by leaks test
		return 0;

	std::string path_config;

	if (argc > 1)
		path_config = argv[1];
	else
		path_config = DEFAULT_CONF;

	string         file = Config::readFile(path_config);
	vector<Config> configs = Config::parseConfig(file);
	Server         servers[configs.size()];
	EpollWrapper   epoll(MAX_EVENTS * configs.size());
	Cookie         cookies;
	channel_t      channelServers[configs.size()];

	size_t i = 0;
	while (i < configs.size())
	{
		Server &server = servers[i];
		Config &config = configs[i];

		server.init(config);

		{ // socket
			channel_t *channel = &channelServers[i];
			channel->fd = server.getServerSocket();
			channel->type = CHANNEL_SOCKET;
			channel->ptr = &servers[i];
			epoll_data_t data = {channel};
			epoll.add(server.getServerSocket(), data, EPOLLIN | EPOLLOUT);
		}

		i++;
	}

	while (running(true))
	{
		int numEvents = epoll.wait(0);
		for (int i = 0; i < numEvents; ++i)
		{
			struct epoll_event &event = epoll.events[i];
			channel_t *channel = reinterpret_cast<channel_t *>(event.data.ptr);

			if (channel->type == CHANNEL_SOCKET)
			{
				Server *server = reinterpret_cast<Server *>(channel->ptr);

				int newClient = server->acceptNewClient();
				{ // client
					channel_t *connection = new channel_t;
					connection->fd = newClient;
					connection->type = CHANNEL_CONNECTION;
					connection->ptr = new Request(newClient);
					epoll_data_t data = {connection};
					epoll.add(newClient, data, EPOLLIN);
				}
				continue;
			}

			Request *request = reinterpret_cast<Request *>(channel->ptr);

			if (event.events & EPOLLERR)
			{
				logError("Epoll error on socket", channel->fd);
				Server::disconnectClient(request);
				delete channel;
				continue;
			}

			if (event.events & (EPOLLRDHUP | EPOLLHUP))
			{
				logError("Client disconnected from socket", channel->fd);
				Server::disconnectClient(request);
				delete channel;
				continue;
			}

			// Request
			if (event.events & EPOLLIN)
			{
				try
				{
					epoll_data_t data = {channel};
					Server::requestClient(request);
					if (request->isParsed())
						epoll.modify(request->getFd(), data, EPOLLOUT);
				}
				catch (std::exception const &e)
				{
					// TODO: handle exception properly...
					logError(e.what());
					Server::disconnectClient(request);
				}
				continue;
			}

			// Response
			if (event.events & EPOLLOUT)
			{
				Server::responseClient(request, cookies);
				epoll.remove(request->getFd());
				Server::disconnectClient(request);
				delete channel;
				continue;
			}
		}
	}
}