#include "Webserv.hpp"

Webserv::Webserv(int port)
{
	// --- Create socket ---
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0)
	{
		std::cerr << "ERROR opening socket" << std::endl;
		exit(1);
	}
	std::cout << "Server socket created: " << server_socket_fd << std::endl;

	// --- Associate socket with port ---
	sockaddr_in server_address = createServerAddress(port);
	if (bind(server_socket_fd, (struct sockaddr *) &server_address,
	         sizeof(server_address)) < 0)
	{
		std::cerr << "ERROR on binding" << std::endl;
		exit(1);
	}

	// --- Set socket to listen for connections ---
	if (listen(server_socket_fd, 5) < 0)
	{
		std::cerr << "ERROR on listen" << std::endl;
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
		std::cerr << "ERROR on getsockname" << std::endl;
		exit(1);
	}
	return ntohs(address.sin_port);
}

void Webserv::run()
{
	sockaddr_in client_addr;
	socklen_t   clilen = sizeof(client_addr);

	while (true)
	{
		// --- Block until client connects and the server accepts the connection
		// ---
		std::cout << "Waiting for connection..." << std::endl;
		int client_socket_fd =
		    accept(server_socket_fd, (struct sockaddr *) &client_addr, &clilen);
		if (client_socket_fd < 0)
		{
			std::cerr << "Error on accept" << std::endl;
			continue;
		}
		std::cout << "Client connected on socket: " << client_socket_fd
		          << std::endl;

		// --- Print request ---
		char buffer[4096];
		int  n = read(client_socket_fd, buffer, 4096 - 1);
		buffer[n] = '\0';
		std::cout << "Received request:\n" << buffer << std::endl;

		// --- Create response body ---
		std::string   filename("src/index.html");
		std::ifstream input_stream(filename.c_str());
		if (!input_stream.is_open())
		{
			std::cerr << "ERROR opening file" << std::endl;
			exit(1);
		}
		std::stringstream file_content;
		file_content << input_stream.rdbuf();
		std::string       body(file_content.str());
		std::stringstream body_size;
		body_size << body.length();

		// --- Create response headers ---
		std::string headers("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");

		// --- Create response ---
		std::string response(headers + "Content-Length : " + body_size.str() +
		                     "\r\n\r\n" + body);

		// --- Send response ---
		write(client_socket_fd, response.c_str(), response.length());
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