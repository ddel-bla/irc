#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <vector>

class Servidor {
	private:
		int puerto;
		std::string password;
		int servidor_fd;
		std::vector<struct pollfd> fds;
	public:
		// Constructor
		Servidor(int puerto, const std::string& password);

		// Métodos principales
		bool iniciar_servidor(); // Inicia el servidor y configura el socket
		void ejecutar(); // Ejecuta el bucle principal para gestionar eventos y clientes

		// Métodos auxiliares
		void aceptar_cliente(); // Acepta nuevas conexiones de clientes
		void procesar_cliente(int cliente_fd); // Procesa datos de clientes existentes
		void eliminar_cliente(int cliente_fd); // Remueve un cliente desconectado de la lista de fds
	};

#endif // IRCSERVER_HPP
