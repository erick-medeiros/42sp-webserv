/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 09:26:18 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/16 11:40:42 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(Config &config, int fd) : config(config), request(fd), fd(fd)
{
}

Connection::~Connection(void) {}

int Connection::disconnect()
{
	logInfo("--- Client disconnected from socket", fd);
	return close(fd);
}
