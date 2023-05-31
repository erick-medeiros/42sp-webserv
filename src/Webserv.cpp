#include "Webserv.hpp"

Webserv::Webserv(int port)
{
	// --- Create socket ---
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0)
	{
		std::cerr << RED "--- Error: socket" RES << std::endl;
		perror("Error"); // cant be used...
		exit(1);
	}
	std::cout << GRAY "Server socket created: " YELLOW << server_socket_fd
	          << RES << std::endl;

	// --- Associate socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(server_socket_fd, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		std::cerr << RED "--- Error: binding" RES << std::endl;
		perror("Error"); // cant be used...
		close(server_socket_fd);
		exit(1);
	}

	// --- Set socket to listen for connections ---
	if (listen(server_socket_fd, 5) < 0)
	{
		std::cerr << RED "--- Error: listen" RES << std::endl;
		perror("Error"); // cant be used...
		exit(1);
	}
}

Webserv::~Webserv()
{
	close(server_socket_fd);
}

int Webserv::getPort()
{
	sockaddr_in address;
	socklen_t   len = sizeof(address);
	if (getsockname(server_socket_fd, (struct sockaddr *) &address, &len) < 0)
	{
		std::cerr << RED "--- Error: getsockname" RES << std::endl;
		perror("Error"); // cant be used...
		exit(1);
	}
	return ntohs(address.sin_port);
}

void displayInitialConnectionMsg(int const port_number)
{
	std::stringstream ss;
	std::string       port;

	ss << port_number;
	if (ss.fail())
	{
		std::cerr << RED "--- Error: converting port number" RES << std::endl;
		perror("Error"); // cant be used...
		return;
	}

	port = ss.str();
	std::cout << BLUE "Waiting new connection in port " CYAN + port << RES
	          << std::endl;
}

void Webserv::run()
{
	sockaddr_in client_addr;
	socklen_t   clilen = sizeof(client_addr);

	while (true)
	{
		displayInitialConnectionMsg(this->getPort());

		// Block until client connects and the server accepts the connection
		int client_socket_fd =
		    accept(server_socket_fd, (struct sockaddr *) &client_addr, &clilen);
		if (client_socket_fd < 0)
		{
			std::cerr << RED "--- Error: while accepting a new connection" RES
			          << std::endl;
			perror("Error"); // cant be used...
			continue;
		}

		std::cout << GREEN "+++ Client connected in socket: " YELLOW
		          << client_socket_fd << RES << std::endl;

		// --- Request ---
		char buff[2048];
		int  bytesWritten = recv(client_socket_fd, buff, sizeof(buff), 0);
		if (bytesWritten == -1)
		{
			std::cerr << RED "--- Error: while receiving data" RES << std::endl;
			perror("Error"); // cant be used...
			continue;
		}
		buff[bytesWritten] = 0;

		if (bytesWritten == 0)
		{
			std::cerr << RED "--- Error: client has closed its connection" RES
			          << std::endl;
			perror("Error"); // cant be used...
			close(client_socket_fd);
			std::cout << RED "--- Client disconnected from socket: " YELLOW
			          << std::endl;
			continue;
		}
		std::cout << GRAY "Request size: " BLUE << bytesWritten << " bytes" RES
		          << std::endl;

		std::string const header = buff;

		try
		{
			Request req(buff);
			req.displayInfo();
		}
		catch (std::exception const &e)
		{
			std::cerr << RED << e.what() << RES << std::endl;
			close(client_socket_fd);
			// TODO: handle exception properly...
			continue;
		}

		// --- Create response body ---
		std::string   filename("src/index.html");
		std::ifstream input_stream(filename.c_str());
		if (!input_stream.is_open())
		{
			std::cerr << RED "--- Error: on open(" CYAN << filename
			          << RED ")" RES << std::endl;
			perror("Error"); // cant be used...
			exit(1);
		}

		std::stringstream file_content;
		file_content << input_stream.rdbuf();
		std::string       body(file_content.str());
		std::stringstream body_size;
		body_size << body.length();

		// --- Create response headers ---
		std::string headers("HTTP/1.1 200 OK\r\nContent-Type: "
		                    "text/html\r\nContent-Length: " +
		                    body_size.str());

		// --- Create response ---
		std::string response(headers + "\r\n\r\n" + body);

		// --- Send response ---
		send(client_socket_fd, response.c_str(), response.length(), 0);

		// --- Close client connection ---
		std::cout << RED "--- Client disconnected from "
		                 "socket: " YELLOW
		          << client_socket_fd << RES << std::endl
		          << std::endl;
		close(client_socket_fd);
	}
}

// --- Helper functions ---
sockaddr_in Webserv::createServerAddress(int port)
{
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	return address;
}
