#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <cstring>    // Para memset y strerror
#include <unistd.h>   // Para close
#include <fcntl.h>    // Para fcntl y O_NONBLOCK
#include <netinet/in.h> // Para sockaddr_in, INADDR_ANY, htons
#include <arpa/inet.h>  // Para funciones relacionadas con direcciones de red
#include <poll.h>       // Para poll
#include <cstdlib> // Para atoi
#include "Cliente.hpp"
#include "IRCServer.hpp"
#include "Evento.hpp"

class Servidor {
	private:
		int puerto;
		std::string password;
		int servidor_fd;
		std::vector<struct pollfd> fds;
		std::map<int, Cliente*> clientes; // Mapeo de FD a objetos Cliente
		std::map<std::string, Canal> canales; // Mapa para gestionar los canales
		Evento evento; // Nuevo atributo para manejar eventos

		void aceptar_cliente();
		void procesar_cliente(int cliente_fd);
		void eliminar_cliente(int cliente_fd);

	public:
		Servidor(int puerto, const std::string& password);
		bool iniciar_servidor();
		void ejecutar();
		std::vector<std::string> obtenerCanalesDeCliente(int cliente_fd) const;
	};

#endif
