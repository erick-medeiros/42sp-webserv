/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 16:04:33 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/14 20:27:04 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "EpollWrapper.hpp"
#include "Server.hpp"

#define DEFAULT_CONF "./config/default.conf"

int main(int argc, char *argv[])
{
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

	size_t i = 0;
	while (i < configs.size())
	{
		Server &server = servers[i];
		Config &config = configs[i];
		server.init(config);

		epoll_data_t data = {server.getRequestSocket()};
		epoll.add(server.getServerSocket(), data, EPOLLIN | EPOLLOUT);

		i++;
	}

	while (true)
	{
		int numEvents = epoll.wait(BLOCK_IND);
		for (int i = 0; i < numEvents; ++i)
		{
			int                 connectionSocket = false;
			struct epoll_event &event = epoll.events[i];
			Request *request = reinterpret_cast<Request *>(event.data.ptr);

			size_t j = 0;
			while (j < configs.size())
			{
				Server &server = servers[j];
				if (request->getFd() == server.getServerSocket())
				{
					int          newClient = server.acceptNewClient();
					epoll_data_t data = {new Request(newClient)};
					epoll.add(newClient, data, EPOLLIN);
					connectionSocket = true;
					break;
				}
				j++;
			}

			if (connectionSocket)
				continue;

			if (event.events & EPOLLERR)
			{
				logError("Epoll error on socket", request->getFd());
				Server::disconnectClient(request);
				continue;
			}

			if (event.events & (EPOLLRDHUP | EPOLLHUP))
			{
				logError("Client disconnected from socket", request->getFd());
				Server::disconnectClient(request);
				continue;
			}

			// Request
			if (event.events & EPOLLIN)
			{
				Server::requestClient(request, epoll);
				continue;
			}

			// Response
			if (event.events & EPOLLOUT)
			{
				Server::responseClient(request, epoll, cookies);
			}
		}
	}
}