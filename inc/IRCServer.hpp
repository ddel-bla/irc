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
#include "Macros.hpp"
#include "Comandos.hpp"
#include "Evento.hpp"

class Servidor
{
	private:
		/* PARAMETROS DE ENTRADA */
		int			puerto;		
		std::string password;

		/* OTROS*/
		int 		servidor_fd;		// FD DEL SERVIDOR
		std::vector<struct pollfd> fds; 
		std::map<int, Cliente*> clientes; // Mapeo de FD a objetos Cliente
		Evento evento; // Nuevo atributo para manejar eventos
		Comando command;

		void aceptar_cliente();
		void procesar_cliente(int cliente_fd);
		void eliminar_cliente(int cliente_fd);
		void manejar_comando(int cliente_fd, const std::string& comando);
		void enviar_mensaje(int cliente_fd, const std::string& mensaje);
		void receiveData(int fd);
		void process_command(std::string command, int fd);
		void quit(std::string command, int fd);

		/* REGISTRATION METHODS */
		void	authenticate(std::string command, Cliente& client);
		void	registerNickname(std::string command, Cliente& client);
		void	registerUsername(std::string command, Cliente& client);
		bool	isValidNickname(const std::string nickname);
		bool	isNicknameTaken(const std::string nickname);

	public:
		Servidor(int puerto, const std::string& password);
		bool iniciar_servidor();
		void ejecutar();
	};

#endif
