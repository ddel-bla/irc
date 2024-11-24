#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include <unistd.h> // Para close() y write()
#include <sys/socket.h> // Para la función send()
#include <iostream>
#include <cerrno>

class Cliente {
	private:
		int socket_fd;
		std::string nickname;
		std::string username;
		std::string hostname;
	public:
		Cliente(int fd, const std::string& nick, const std::string& user, const std::string& host);
		~Cliente();

		// Métodos getter
		int getSocketFD() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getHostname() const;

		// Métodos setter
		void setNickname(const std::string& nick);

		// Enviar un mensaje al cliente
		void enviarMensaje(const std::string& mensaje) const;
};

#endif
