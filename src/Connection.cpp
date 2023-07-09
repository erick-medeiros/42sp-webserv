/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mi <mi@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 09:26:18 by eandre-f          #+#    #+#             */
/*   Updated: 2023/07/08 10:39:28 by mi               ###   ########.fr       */
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
	log.info("--- Client disconnected from socket" + utils::to_string(fd));
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
		log.error("--- Error: accept", strerror(errno));
		return -1;
	}
	// --- Set non-blocking ---
	if (!setNonBlocking(clientSocket))
	{
		log.error("--- Error: Set non-blocking");
		return -1;
	}
	log.info("+++ New connection accepted on socket: " +
	         utils::to_string(clientSocket));
	return clientSocket;
}

int Connection::sendHttpResponse()
{
	response.prepareMessage();
	std::string message = response.getMessage();

	log.debug("Sending response:\n" + message);

	if (send(fd, message.c_str(), message.size(), 0) < 0)
	{
		throw std::runtime_error("Error sending response");
	}
	if (response.getStatusCode() == HttpStatus::PAYLOAD_TOO_LARGE)
	{
		// Give some time to the client to read the response
		// before closing the connection
		sleep(1);
	}
	return 0;
}