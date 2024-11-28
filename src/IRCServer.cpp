#include "IRCServer.hpp"
#include "Evento.hpp"
#include <iostream>
#include <cstring>   // Para strerror
#include <unistd.h>  // Para close

// Constructor
Servidor::Servidor(int puerto, const std::string& password): puerto(puerto), password(password), servidor_fd(-1) {}

// Inicia el servidor
bool Servidor::iniciar_servidor(void)
{
	servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (servidor_fd < 0) {
		std::cerr << "Error al crear el socket: " << strerror(errno) << std::endl;
		return false;
	}

	int opt = 1;
	if (setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "Error en setsockopt: " << strerror(errno) << std::endl;
		close(servidor_fd);
		return false;
	}

	if (fcntl(servidor_fd, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << "Error al configurar el modo no bloqueante: " << strerror(errno) << std::endl;
		close(servidor_fd);
		return false;
	}

	sockaddr_in direccion;
	std::memset(&direccion, 0, sizeof(direccion));
	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = INADDR_ANY;
	direccion.sin_port = htons(puerto);

	if (bind(servidor_fd, (struct sockaddr*)&direccion, sizeof(direccion)) < 0) {
		std::cerr << "Error en bind: " << strerror(errno) << std::endl;
		close(servidor_fd);
		return false;
	}

	if (listen(servidor_fd, 5) < 0) {
		std::cerr << "Error en listen: " << strerror(errno) << std::endl;
		close(servidor_fd);
		return false;
	}

	std::cout << "Servidor iniciado en el puerto " << puerto << std::endl;
	return true;
}

// Bucle principal del servidor
void Servidor::ejecutar() {
	struct pollfd pfd;
	pfd.fd = servidor_fd;
	pfd.events = POLLIN;

	fds.push_back(pfd);
	while (true) {
		int ret = poll(fds.data(), fds.size(), -1);
		if (ret < 0) {
			std::cerr << "Error en poll: " << strerror(errno) << std::endl;
			break;
		}

		for (size_t i = 0; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == servidor_fd) {
					aceptar_cliente();
				} else {
					receiveData(fds[i].fd);
				}
			}
		}
	}
}

// Acepta un nuevo cliente
void Servidor::aceptar_cliente() {
	sockaddr_in direccion_cliente;
	socklen_t tam_direccion = sizeof(direccion_cliente);
	int cliente_fd = accept(servidor_fd, (struct sockaddr*)&direccion_cliente, &tam_direccion);

	if (cliente_fd < 0) {
		if (errno != EWOULDBLOCK) {
			std::cerr << "Error al aceptar cliente: " << strerror(errno) << std::endl;
		}
		return;
	}

	if (fcntl(cliente_fd, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << "Error al configurar cliente en modo no bloqueante: " << strerror(errno) << std::endl;
		close(cliente_fd);
		return;
	}
	
	// Crear un nuevo cliente con valores predeterminados
	Cliente* nuevo_cliente = new Cliente(cliente_fd);
	clientes[cliente_fd] = nuevo_cliente;

	// Notificar conexión usando Evento
	evento.notificarConexion(cliente_fd, nuevo_cliente->getNickname(), clientes);

	// Agregar el cliente a la lista de fds para poll
	struct pollfd nuevo_cliente_fd;
	nuevo_cliente_fd.fd = cliente_fd;
	nuevo_cliente_fd.events = POLLIN;
	fds.push_back(nuevo_cliente_fd);

	std::cout << "Un nuevo cliente se ha conectado. FD: " << cliente_fd << std::endl;
}

// Procesa mensajes de un cliente
void Servidor::procesar_cliente(int cliente_fd) {
	char buffer[512];
	int bytes_leidos = recv(cliente_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_leidos <= 0) {
		if (bytes_leidos == 0) {
			// Cliente desconectado
			Cliente* cliente = clientes[cliente_fd];
			evento.notificarDesconexion(cliente_fd, cliente->getNickname(), clientes);
		} else {
			std::cerr << "Error al recibir datos: " << strerror(errno) << std::endl;
		}
		close(cliente_fd);
		eliminar_cliente(cliente_fd);
		return;
	}

	buffer[bytes_leidos] = '\0';
    evento.enviarMensajeGlobal(buffer, clientes);
	std::cout << "Mensaje recibido de FD " << cliente_fd << ": " << buffer << std::endl;

}

// Elimina un cliente desconectado
void Servidor::eliminar_cliente(int cliente_fd) {
	std::map<int, Cliente*>::iterator it = clientes.find(cliente_fd);
	if (it != clientes.end()) {
		delete it->second; // Liberar memoria del cliente
		clientes.erase(it); // Eliminar del mapa
	}

	for (size_t i = 0; i < fds.size(); ++i) {
		if (fds[i].fd == cliente_fd) {
			fds.erase(fds.begin() + i);
			break;
		}
	}
}

void Servidor::enviar_mensaje(int cliente_fd, const std::string& mensaje) {
    std::string mensaje_completo = mensaje + "\r\n";
    send(cliente_fd, mensaje_completo.c_str(), mensaje_completo.size(), 0);
}

void Servidor::receiveData(int fd)
{
	char		buffer[512];
	size_t		bytes;
	Cliente*	cliente = clientes[fd];
	std::vector<std::string> commands;

	memset(buffer, 0, sizeof(buffer));
	bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
	{
		// CLIENT DISCONECTION
		evento.notificarDesconexion(fd, cliente->getNickname(), clientes);
		close(fd);
		eliminar_cliente(fd);
		return;
	}
    
    cliente->setBuffer(buffer);

    if (cliente->getBuffer().find_first_of(CRLF) == std::string::npos)
        return;

    // SPLIT BY '\r\n'
    commands = Utils::split(cliente->getBuffer(), CRLF);
	
    // PROCESS EACH COMMAND
    for (size_t i = 0; i < commands.size(); i++)
        process_command(commands[i], fd);

    cliente->clearBuffer();
}

void Servidor::process_command(std::string command, int fd)
{
	std::vector<std::string> split_command;
	Cliente	*cliente = clientes[fd];

	if (command.empty())
		return ;
	
	split_command = Utils::splitBySpaces(command);

	// EMPTY VECTOR
	if (!split_command.size())
		return ;
	
	if (split_command[0] == PASS)
		authenticate(command, *cliente);
	else if (split_command[0] == USER)
	{
		std::cout << "Procesando username..." << std::endl;
		registerUsername(command, *cliente);
	}
	else if (split_command[0] == NICK)
	{
		std::cout << "Procesando nickname..." << std::endl;
		registerNickname(command, *cliente);
	}
	else if (split_command[0] == QUIT)
		quit(command, fd);
	else if (cliente->isRegistred()) // Si está registrado puede ejecutar otros comandos
	{
		std::cout << "Puedes ejecutar otros comandos" << std::endl;	
	}
	else if (!cliente->isRegistred())
		std::cout << cliente->isRegistred() << ERR_NOTREGISTERED(cliente->getNickname()) << std::endl;
	(void) fd;
}

void Servidor::quit(std::string command, int fd)
{
	std::vector<std::string> split_command;

	split_command = Utils::splitBySpaces(command);

	std::cout << "Client disconnected" << std::endl;
	close(fd);
	eliminar_cliente(fd);
}

void	Servidor::authenticate(std::string command, Cliente& client)
{
	  std::vector<std::string>    split_command;
    std::string                 enteredPassword;

    split_command = Utils::splitBySpaces(command);
    if (split_command.size() == 2 && !client.isRegistred())
    {
        // SKIP 'PASS'
		enteredPassword = split_command[1];

		// REMOVE ':' -- PASS :password
		Utils::removeLeadingChar(enteredPassword, ':');

        if (enteredPassword == this->password)
        {
            client.setAutenticate(true);
            std::cout << "Contraseña correcta... " << client.isAutenticate() << std::endl;
        }
        else
        {
            std::cout << ERR_PASSWDMISMATCH(client.getNickname()) << std::endl;
            client.setAutenticate(false);
        }
    }
    else if (client.isRegistred())
        std::cout << ERR_ALREADYREGISTERED(client.getNickname()) << std::endl;
	else
		std::cout << ERR_NEEDMOREPARAMS(client.getNickname()) << std::endl;
}

void	Servidor::registerNickname(std::string command, Cliente& client)
{
	std::vector<std::string>	split_command;
	size_t	command_len;
	std::string	nickname;
	std::string	oldNickname;

	// DEBE ESTAR AUTENTICADO (PASS)
	if (client.isAutenticate())
	{
		split_command = Utils::splitBySpaces(command);
		command_len = split_command.size(); 
		if (command_len == 2)
		{
			// SKIP 'NICK'
			nickname = split_command[1];
			
			// REMOVE ':' NICK :wii
			Utils::removeLeadingChar(nickname, ':');

			if (!isValidNickname(nickname))
				std::cout << ERR_ERRONEUSNICK(nickname) << std::endl;
			else if (isNicknameTaken(nickname))
				std::cout << ERR_NICKCOLLISION(nickname) << std::endl;
			else
			{	
				// YA TIENE NICK --> CAMBIAR NICK
				if (!client.getNickname().empty() && nickname != client.getNickname())
					oldNickname = client.getNickname();
				
				client.setNickname(nickname);
				
				// ACTUALIZAR CANALES

				// CAMBIO NICK -- MENSAJE EXITO
				if (!client.getUsername().empty())
				{
					client.setRegistred(true);
					std::cout << RPL_CONNECTED(client.getNickname()) << std::endl;
					if (!oldNickname.empty())
						std::cout << oldNickname << ": Nickname changed!" << std::endl;	 //OPTIONAL
				}
			}
		}
		else if (command_len < 2) 	// Not enough params
			std::cout << ERR_NONICKNAME(std::string("*")) << std::endl;
		else if (command_len > 2)	// Spaces not allowed
			std::cout << ERR_ERRONEUSNICK(std::string("*")) << std::endl;
	}
	else
		std::cout << ERR_NOTREGISTERED(std::string("*")) << std::endl;
	
}

void	Servidor::registerUsername(std::string command, Cliente& client)
{
	std::vector<std::string>	split_command;
	size_t	command_len;

	// DEBE ESTAR AUTENTICADO (PASS)
	if(client.isAutenticate())
	{
		split_command = Utils::splitBySpaces(command);
		command_len = split_command.size();
		if (command_len < 5)
			std::cout << ERR_NEEDMOREPARAMS(client.getNickname());
		else if (!client.getUsername().empty())
			std::cout << ERR_ALREADYREGISTERED(client.getNickname()) << std::endl;
		else
		{
			client.setUsername(split_command[1]);
			if (!client.getNickname().empty())
			{
				client.setRegistred(true);
				std::cout << RPL_CONNECTED(client.getNickname()) << std::endl;	
			}
		}
	}
	else
		std::cout << ERR_NOTREGISTERED(std::string("*")) << std::endl;
}

bool	Servidor::isValidNickname(const std::string nickname)
{	
	// VALID CHARACTERS
    if (nickname.empty() || nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
        return (false);

    for (size_t i = 0; i < nickname.size(); i++) {
        if (!std::isalnum(nickname[i]) && nickname[i] != '_')
            return (false);
    }

    // // UNIQUE (not in use)
    // for (std::map<int, Cliente*>::const_iterator it = clientes.begin(); it != clientes.end(); ++it){
    //     Cliente* cliente = it->second;
    //     if (cliente && cliente->getNickname() == nickname) {
    //         return false;
    //     }
    // }

    return (true);
}

bool	Servidor::isNicknameTaken(const std::string nickname)
{
	std::map<int, Cliente*>:: const_iterator it;
	Cliente* cliente;

	for (it = clientes.begin(); it != clientes.end(); ++it)
	{
        cliente = it->second;
        if (cliente && cliente->getNickname() == nickname) {
            return (true);
        }
    }

	return (false);
}
