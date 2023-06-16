/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 09:26:18 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/16 11:40:44 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <iostream>

class Connection
{
  public:
	Config  &config;
	Request  request;
	Response response;
	int      fd;

	Connection(Config &config, int fd);
	~Connection(void);

	int disconnect();

  private:
};

#endif /* CONNECTION_HPP */
