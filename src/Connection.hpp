/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 09:26:18 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 00:42:57 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <iostream>

class Server;
class Config;

class Connection
{
  public:
	int      fd;
	Server  &server;
	Config  &config;
	Request  request;
	Response response;

	Connection(Server &server);
	~Connection(void);

	int sendHttpResponse(void);

	int disconnect();

  private:
	static int acceptNewClient(int serverSocket);
};

#endif /* CONNECTION_HPP */
