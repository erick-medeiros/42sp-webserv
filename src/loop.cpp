/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 10:55:41 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 00:44:39 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Connection.hpp"
#include "EpollWrapper.hpp"
#include "Request.hpp"
#include "Server.hpp"

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
					channel_t  *channelConnection = new channel_t;
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
				logError("Epoll error on socket", connection->fd);
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
					connection->request.parse(rawRequest);
					std::cout << connection->request << std::endl;
					if (connection->request.isParsed())
					{
						epoll_data_t data = {channel};
						epoll.modify(connection->fd, data, EPOLLOUT);
					}
				}
				catch (std::exception const &e)
				{
					// TODO: handle exception properly...
					logError(e.what());
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

// TODO: Ideia para gerenciar cookies ao invés de usar essa flag:
// Usuario faz login, cria um Cookie com id e usuário, guarda o id e usuário na lista
// de cookies Quando o usuario faz uma nova requisição, verifica se aquela request
// tem um cookie com um id Acho que podia rodar isso em todos request essa checagem
// se tem um cookie que é igual a um dos cookies que estão na lista de cookies, tipo
// um checkCookies

// // Cookie test
// if (FEATURE_FLAG_COOKIE)
// {
// 	string username = Cookie::getUsername(request);
// 	if (username == "")
// 	{
// 		string value = Cookie::getValueCookie(request, "session");
// 		if (cookies.get(value) != "")
// 		{
// 			response.setStatus(200);
// 			response.setBody("username " + cookies.get(value));
// 		}
// 	}
// 	else
// 	{
// 		string session = cookies.generateSession();
// 		cookies.set(session, username);
// 		response.setHeader("Set-Cookie", "session=" + session + ";path=/");
// 	}
// }