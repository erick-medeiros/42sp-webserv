/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 09:26:18 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/27 08:13:56 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(Server &server)
    : fd(Connection::acceptNewClient(server.getServerSocket())), server(server),
      config(server.getConfig())
{
}

Connection::~Connection(void) {}

int Connection::disconnect()
{
	logInfo("--- Client disconnected from socket", fd);
	return close(fd);
}

int Connection::acceptNewClient(int serverSocket)
{
	sockaddr_in clientAddr;
	socklen_t   clilen = sizeof(clientAddr);

	int clientSocket =
	    accept(serverSocket, (struct sockaddr *) &clientAddr, &clilen);
	if (clientSocket == -1)
	{
		logError("--- Error: accept", strerror(errno));
		exit(1);
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(clientSocket))
	{
		logError("--- Error: Set non-blocking");
		exit(1);
	}
	logSuccess("+++ New connection accepted on socket", clientSocket);
	return clientSocket;
}

int Connection::sendHttpResponse()
{
	response.prepareMessage();
	std::string message = response.getMessage();
	logWarning("Sending response:\n" + message);

	if (send(fd, message.c_str(), message.size(), 0) < 0)
	{
		throw std::runtime_error("Error sending response");
	}
	return 0;
}
