#include "IRCServer.hpp"
#include "Message.hpp"
#include <iostream>
#include <cstring>   // For strerror
#include <unistd.h>  // For close

// Constructor
IRCServer::IRCServer(int port, const std::string& password)
	: port(port), password(password), server_fd(-1) {}

// Starts the server
bool IRCServer::startServer() {
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		return false;
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "Error in setsockopt: " << strerror(errno) << std::endl;
		close(server_fd);
		return false;
	}

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << "Error setting non-blocking mode: " << strerror(errno) << std::endl;
		close(server_fd);
		return false;
	}

	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		std::cerr << "Error in bind: " << strerror(errno) << std::endl;
		close(server_fd);
		return false;
	}

	if (listen(server_fd, 5) < 0) {
		std::cerr << "Error in listen: " << strerror(errno) << std::endl;
		close(server_fd);
		return false;
	}

	std::cout << "Server started on port " << port << std::endl;
	return true;
}

// Main server loop
void IRCServer::run() {
	struct pollfd pfd;
	pfd.fd = server_fd;
	pfd.events = POLLIN;

	fds.push_back(pfd);
	while (true) {
		int ret = poll(fds.data(), fds.size(), -1);
		if (ret < 0) {
			std::cerr << "Error in poll: " << strerror(errno) << std::endl;
			break;
		}

		for (size_t i = 0; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == server_fd) {
					acceptClient();
				} else {
					processClient(fds[i].fd);
				}
			}
		}
	}
}
void IRCServer::acceptClient() {
    sockaddr_in client_address;
    socklen_t address_length = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &address_length);

    if (client_fd < 0) {
        if (errno != EWOULDBLOCK) {
            std::cerr << "Error accepting client: " << strerror(errno) << std::endl;
        }
        return;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error setting client to non-blocking mode: " << strerror(errno) << std::endl;
        close(client_fd);
        return;
    }

    // Crear un cliente con valores predeterminados
    Client* new_client = new Client(client_fd, "defaultNick", "defaultUser", "defaultHost");
    clients[client_fd] = new_client;

	const std::string dummy_channel_name = "#dummy";

	// Verifica si el canal #dummy existe, si no, lo inserta
	std::map<std::string, Channel>::iterator it = channels.find(dummy_channel_name);
	if (it == channels.end()) {
		// El canal no existe, se inserta usando insert
		channels.insert(std::make_pair(dummy_channel_name, Channel(dummy_channel_name)));
		it = channels.find(dummy_channel_name);  // Obtener el iterador después de la inserción
	}

	// Agrega el cliente al canal #dummy
	it->second.addMember(client_fd, new_client);

    // Notificar la conexión al canal
	std::string join_message = new_client->getNickname() + " has joined " + dummy_channel_name + "\n";

    message.sendToChannel(dummy_channel_name, join_message, -1, channels);

    // Agregar el cliente a la lista de poll fds
    struct pollfd new_client_fd;
    new_client_fd.fd = client_fd;
    new_client_fd.events = POLLIN;
    fds.push_back(new_client_fd);

    std::cout << "New client connected. FD: " << client_fd << std::endl;
}

// Processes messages from a client
void IRCServer::processClient(int client_fd) {
	char buffer[512];
	int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0) {
		if (bytes_read == 0) {
			// Client disconnected
			Client* client = clients[client_fd];
			std::string msg = "TO DO " + std::string(client->getNickname()) + " is disconnected";
			message.sendToAll(msg, client_fd, channels);
		} else {
			std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
		}
		close(client_fd);
		removeClient(client_fd);
		return;
	}

	buffer[bytes_read] = '\0';
	message.sendToAll(buffer, client_fd, channels);
	std::cout << "Message received from FD " << client_fd << ": " << buffer;
}

// Removes a disconnected client
void IRCServer::removeClient(int client_fd) {
	std::map<int, Client*>::iterator it = clients.find(client_fd);
	if (it != clients.end()) {
		delete it->second; // Free client memory
		clients.erase(it); // Remove from map
	}

	for (size_t i = 0; i < fds.size(); ++i) {
		if (fds[i].fd == client_fd) {
			fds.erase(fds.begin() + i);
			break;
		}
	}
}

// Retrieves the channels a client is part of
std::vector<std::string> IRCServer::getClientChannels(int client_fd) const {
	std::vector<std::string> clientChannels;

	for (std::map<std::string, Channel>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		if (it->second.isMember(client_fd)) {
			clientChannels.push_back(it->first);
		}
	}
	return clientChannels;
}
