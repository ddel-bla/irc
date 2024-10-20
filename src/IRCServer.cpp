#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "../inc/IRCServer.hpp"

#define PORT 6667
#define MAX_CLIENTS 100

IRCServer::IRCServer() : server_socket(-1) {}

IRCServer::~IRCServer() {
	if (server_socket != -1) {
		close(server_socket);
	}
}

bool IRCServer::start() {
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		std::cerr << "Error al crear el socket" << std::endl;
		return false;
	}

	// Hacer que el socket sea no bloqueante
	fcntl(server_socket, F_SETFL, O_NONBLOCK);

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Error en bind" << std::endl;
		return false;
	}

	if (listen(server_socket, MAX_CLIENTS) < 0) {
		std::cerr << "Error en listen" << std::endl;
		return false;
	}

	// Agregar el socket del servidor a poll_fds
	pollfd server_pollfd;
	server_pollfd.fd = server_socket;
	server_pollfd.events = POLLIN; // Escuchar conexiones entrantes
	poll_fds.push_back(server_pollfd);

	std::cout << "Servidor IRC escuchando en el puerto " << PORT << std::endl;
	return true;
}

void IRCServer::run() {
	while (true) {
		int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
		if (poll_count < 0) {
			std::cerr << "Error en poll()" << std::endl;
			break;
		}

		for (size_t i = 0; i < poll_fds.size(); i++) {
			if (poll_fds[i].revents & POLLIN) {
				if (poll_fds[i].fd == server_socket) {
					// Aceptar nueva conexión
					accept_client();
				} else {
					// Manejar comunicación con un cliente
					handle_client(poll_fds[i].fd);
				}
			}
		}
	}
}

void IRCServer::accept_client() {
	sockaddr_in client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

	if (client_socket < 0) {
		std::cerr << "Error al aceptar cliente" << std::endl;
		return;
	}

	// Hacer que el nuevo socket sea no bloqueante
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	// Agregar cliente a la lista de poll_fds
	pollfd client_pollfd;
	client_pollfd.fd = client_socket;
	client_pollfd.events = POLLIN;
	poll_fds.push_back(client_pollfd);

	// Agregar cliente a la lista de clientes
	clients[client_socket] = Client(client_socket);

	std::cout << "Nuevo cliente conectado" << std::endl;
}

void IRCServer::handle_client(int client_socket) {
	char buffer[1024];
	ssize_t nbytes = recv(client_socket, buffer, sizeof(buffer), 0);

	if (nbytes <= 0) {
		// Error o desconexión del cliente
		if (nbytes == 0) {
			std::cout << "Cliente desconectado" << std::endl;
		} else {
			std::cerr << "Error al recibir datos del cliente" << std::endl;
		}
		close_client(client_socket);
	} else {
		buffer[nbytes] = '\0';
		std::cout << "Mensaje recibido: " << buffer << std::endl;
		// Procesar el mensaje recibido (autenticación, comandos IRC, etc.)
	}
}

void IRCServer::close_client(int client_socket) {
	close(client_socket);

	// Remover el cliente de poll_fds y de la lista de clientes
	for (size_t i = 0; i < poll_fds.size(); i++) {
		if (poll_fds[i].fd == client_socket) {
			poll_fds.erase(poll_fds.begin() + i);
			break;
		}
	}

	clients.erase(client_socket);
}
