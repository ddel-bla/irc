#include "IRCServer.hpp"

/* PARAMETRIZED CONSTRUCTOR */
IRCServer::IRCServer(int port, const std::string& password): port(port), password(password), server_fd(-1) {}

/* METHODS */
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
					receiveData(fds[i].fd);
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
    Client* new_client = new Client(client_fd);
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

void IRCServer::receiveData(int fd)
{
	char		buffer[512];
	size_t		bytes;
	Client*		cliente = clients[fd];
	std::vector<std::string> commands;

	memset(buffer, 0, sizeof(buffer));
	bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
	{
		// CLIENT DISCONECTION
		std::cout << "notify desconection.." << std::endl;
		close(fd);
		removeClient(fd);
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

void IRCServer::process_command(std::string command, int fd)
{
	std::vector<std::string> split_command;
	Client	*cliente = clients[fd];

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

void IRCServer::quit(std::string command, int fd)
{
	std::vector<std::string> split_command;

	split_command = Utils::splitBySpaces(command);

	std::cout << "Client disconnected" << std::endl;
	close(fd);
	removeClient(fd);
}

void	IRCServer::authenticate(std::string command, Client& client)
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

void	IRCServer::registerNickname(std::string command, Client& client)
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

void	IRCServer::registerUsername(std::string command, Client& client)
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

bool	IRCServer::isValidNickname(const std::string nickname)
{	
	// VALID CHARACTERS
    if (nickname.empty() || nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
        return (false);

    for (size_t i = 0; i < nickname.size(); i++) {
        if (!std::isalnum(nickname[i]) && nickname[i] != '_')
            return (false);
    }

    return (true);
}

bool	IRCServer::isNicknameTaken(const std::string nickname)
{
	std::map<int, Client*>:: const_iterator it;
	Client* cliente;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
        cliente = it->second;
        if (cliente && cliente->getNickname() == nickname) {
            return (true);
        }
    }

	return (false);
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
