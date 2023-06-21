/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mi <mi@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 09:26:18 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/18 15:51:19 by mi               ###   ########.fr       */
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
	int             fd;
	Server		   &server;
	Config         &config;
	Request         request;
	Response        response;

	Connection(Server &server);
	~Connection(void);

	int disconnect();

  private:
	static int acceptNewClient(int serverSocket);
};

#endif /* CONNECTION_HPP */
