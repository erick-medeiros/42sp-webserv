/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 10:55:41 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/19 09:24:12 by eandre-f         ###   ########.fr       */
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
	// TODO: Que tal inicializar os servers no loop e não aqui?
	// Seria tudo no construtor e não precisaria o init
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
			Request    *request = reinterpret_cast<Request *>(&connection->request);

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
					std::string rawRequest = Server::getRequestData(request);
					request->parse(rawRequest);
					std::cout << *request << std::endl;
					if (request->isParsed())
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
				Server server = connection->server;
				Response response = server.handleRequest(*request, *connection);
				response.sendHttpResponse();
				removeConnection(channel, epoll);
				continue;
			}
		}
	}
	return 0;
}

// TODO:
// Acho que podia ter um SessionManager que gerencia as sessões e cookies
// ex: Usuario faz login, o SessionManager cria uma session relacionada com
// o usuario, e cria um cookie com o id da sessão, guarda no map de cookies, e
// envia de volta pro usuario. Quando o usuario faz uma nova requisição, o
// SessionManager verifica se aquela request tem um cookie, se tiver, ele verifica
// se o cookie é valido, se for, ele retorna o usuario relacionado com aquele cookie
// Aí não precisa passar o Cookie nas funções, esse SessionManager checa a request e
// só isso

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