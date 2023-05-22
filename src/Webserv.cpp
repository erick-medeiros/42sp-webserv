#include "Webserv.hpp"

Webserv::Webserv(int port)
{
    // --- Create socket ---
    server_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (server_socket_fd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    std::cout << "Server socket created: " << server_socket_fd << std::endl;

    // --- Associate socket with port ---
    sockaddr_in server_address = createServerAddress(port);
    if (bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // --- Set socket to listen for connections ---
    if (listen(server_socket_fd, 5) < 0) {
        perror("ERROR on listen");
        exit(1);
    }
}

Webserv::~Webserv()
{
    close(server_socket_fd);
}

void Webserv::run()
{
    sockaddr_in client_addr;
    socklen_t clilen = sizeof(client_addr);

    while (true)
    {
        // --- Block until client connects and the server accepts the connection ---
        std::cout << "Waiting for connection..." << std::endl;
        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &clilen);
        if (client_socket_fd < 0)
        {
            std::cerr << "Error on accept" << std::endl;
            continue;
        }
        std::cout << "Client connected on socket: " << client_socket_fd << std::endl;

        // --- Print request ---
        char buffer[4096];   
        int n = read(client_socket_fd, buffer, 4096 - 1);
        buffer[n] = '\0';
        std::cout << "Received request:\n" << buffer << std::endl;

        // --- Send response ---
        close(client_socket_fd);
    }
}

// --- Helper functions ---
sockaddr_in Webserv::createServerAddress(int port) {
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    return address;
}