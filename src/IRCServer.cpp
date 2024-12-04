#include "IRCServer.hpp"

bool IRCServer::signal = false;

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
	struct	pollfd	pfd;
	
	pfd.fd = server_fd;
	pfd.events = POLLIN;
	fds.push_back(pfd);
	while (!IRCServer::signal) {
		int ret = poll(fds.data(), fds.size(), 1000); // Timeout 1 sec
		if (ret < 0  && !IRCServer::signal) {
			std::cerr << "Error in poll: " << strerror(errno) << std::endl;
			break;
		}

        // REGISTER TIMEOUT
        checkRegistrationTimeout();

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

void IRCServer::RemoveFds(int fd){
	for (size_t i = 0; i < this->fds.size(); i++){
		if (this->fds[i].fd == fd)
			{this->fds.erase(this->fds.begin() + i); return;}
	}
}

void IRCServer::acceptClient()
{
    sockaddr_in	client_address;
    socklen_t	address_length = sizeof(client_address);
    int			client_fd = accept(server_fd, (struct sockaddr*)&client_address, &address_length);

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

    // Agregar el cliente a la lista de poll fds
    struct pollfd new_client_fd;
    new_client_fd.fd = client_fd;
    new_client_fd.events = POLLIN;
    fds.push_back(new_client_fd);

    std::cout << "New client connected. FD: " << client_fd << std::endl;
}

// Processes messages from a client
void IRCServer::processClient(int client_fd)
{
	char buffer[512];
	int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0) {
		if (bytes_read == 0) {
			// Client disconnected
			Client* client = clients[client_fd];
			std::string msg = "TO DO " + std::string(client->getNickname()) + " is disconnected";
			message.sendToAll(msg, channels, client_fd);
		} else {
			std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
		}
		close(client_fd);
		removeClient(client_fd);
		return;
	}

	buffer[bytes_read] = '\0';
	message.sendToAll(buffer, channels, client_fd);
	std::cout << "Message received from FD " << client_fd << ": " << buffer;
}

// Removes a disconnected client
void IRCServer::removeClient(int client_fd)
{
	std::map<int, Client*>::iterator it = clients.find(client_fd);
    
    if (it != clients.end()) {
        // Liberar la memoria del cliente
        it->second->setNickname("");
		it->second->setUsername("");
        
        // Eliminar la entrada del mapa
        //clients.erase(it);
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
	Client*		client = clients[fd];
	std::vector<std::string> commands;

	memset(buffer, 0, sizeof(buffer));
	bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
	{
		if (bytes == 0) {
			// Client disconnected
			std::string msg = "TO DO " + std::string(client->getNickname()) + " is disconnected";
			message.sendToAll(msg, channels, fd);
		} else {
			std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
		}
		removeClient(fd);
		return;
	}
    
    client->setBuffer(buffer);

    if (client->getBuffer().find_first_of(CRLF) == std::string::npos)
        return;

    // SPLIT BY '\r\n'
    commands = Utils::split(client->getBuffer(), CRLF);
	
    // PROCESS EACH COMMAND
    for (size_t i = 0; i < commands.size(); i++)
		process_command(commands[i], fd);
        
	if (client)
    	client->clearBuffer();
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
		if (split_command[0] == PRIVMSG)
			privMsg(command, *cliente);
		if (split_command[0] == JOIN)
			join(command, *cliente);
	}
	else if (!cliente->isRegistred())
		std::cout << cliente->isRegistred() << ERR_NOTREGISTERED(cliente->getNickname()) << std::endl;
	
}

void IRCServer::quit(std::string command, int fd)
{
	std::vector<std::string> split_command;

	split_command = Utils::splitBySpaces(command);

	std::cout << "Client disconnected" << std::endl;
	close(fd);
	RemoveFds(fd);
	removeClient(fd);
}

/* REGISTRATION PROCESS */
void	IRCServer::checkRegistrationTimeout(void)
{
    std::time_t now = std::time(NULL);
    
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ) {
        Client* cliente = it->second;

        if (!cliente->isRegistred() && std::difftime(now, cliente->getConnectionTime()) > REGISTRATION_TIMEOUT) {
			std::cout << "Cliente no completó el registro a tiempo. Desconectando..." << std::endl;
			close(it->first);
			RemoveFds(it->first);
            delete cliente;
            
            std::map<int, Client*>::iterator toErase = it;
            ++it;
			clients.erase(toErase);
        } else {
            ++it; // Avanzar al siguiente iterador si no se borra
        }
    }
}

void	IRCServer::authenticate(std::string command, Client& client)
{
	// 1. Split command
	std::vector<std::string> split_command= Utils::splitBySpaces(command);;
    std::string enteredPassword;

    if (split_command.size() == 2 && !client.isRegistred())
    {
        // 2. Skip 'PASS' & REMOVE ':' -- PASS :password
		enteredPassword = Utils::removeLeadingChar(split_command[1], ':');
		
		// 3. Valid passwd
        if (enteredPassword == this->password)
            client.setAutenticate(true);
        // 3. Wrong passwd
		else
        {
            message.sendToClient(client.getFd(), ERR_PASSWDMISMATCH(client.getNickname()));
            client.setAutenticate(false);
        }
    }
    else if (client.isRegistred())	// ALREADY REGISTERED
        message.sendToClient(client.getFd(), ERR_ALREADYREGISTERED(client.getNickname()));
	else							// NEED MORE PARAMS
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
}

void	IRCServer::registerNickname(std::string command, Client& client)
{
	//1. Authenticated (PASS)
	if (client.isAutenticate())
	{
		std::vector<std::string> split_command = Utils::splitBySpaces(command);
		size_t command_len = split_command.size(); 
		if (command_len == 2)
		{
			//2. Skip 'NICK' & remove ':' NICK :wii
			std::string	nickname = Utils::removeLeadingChar(split_command[1], ':');

			if (!isValidNickname(nickname))
				message.sendToClient(client.getFd(), ERR_ERRONEUSNICK(nickname));
			else if (isNicknameTaken(nickname))
				message.sendToClient(client.getFd(), ERR_NICKCOLLISION(nickname));
			else
			{	
				std::string	oldNickname;
				
				// 3. Has already a nick --> changing nickname
				if (!client.getNickname().empty() && nickname != client.getNickname())
					oldNickname = client.getNickname();
				
				client.setNickname(nickname);
				
				// 4. Channel clients update
				updateChannelsClientNickname(client.getFd(), nickname);

				// 5. Set registered and success msg
				if (!client.getUsername().empty())
				{
					if (!client.isRegistred()) // If already registered dont send
						message.sendToClient(client.getFd(), RPL_CONNECTED(client.getNickname()));
					client.setRegistred(true);
					if (!oldNickname.empty())
					{
						message.sendToClient(client.getFd(), RPL_CHANGEDNICK(oldNickname));
					}
				}
			}
		}
		else if (command_len < 2) 	// Not enough params
			message.sendToClient(client.getFd(), ERR_NONICKNAME(std::string("*")));
		else if (command_len > 2)	// Spaces not allowed
			message.sendToClient(client.getFd(), ERR_ERRONEUSNICK(std::string("*")));
	}
	else
		std::cout << ERR_NOTREGISTERED(std::string("*")) << std::endl;
}

void	IRCServer::registerUsername(std::string command, Client& client)
{
	// 1. Autenticated
	if(client.isAutenticate())
	{
		// 2. Split command
		std::vector<std::string> split_command = Utils::splitBySpaces(command);
		size_t command_len = split_command.size();
		if (command_len < 5)					// NEED MORE PARAMS
			message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
		else if (!client.getUsername().empty())	// ALREADY REGISTERED
			message.sendToClient(client.getFd(), ERR_ALREADYREGISTERED(client.getNickname()));
		else
		{
			// 3. Set username
			client.setUsername(split_command[1]);
			
			// 4. Set registered to true if NICK and USER completed
			if (!client.getNickname().empty())
			{
				client.setRegistred(true);
				message.sendToClient(client.getFd(), RPL_CONNECTED(client.getNickname()));	
			}
		}
	}
	else		// NO AUTENTICATED
		message.sendToClient(client.getFd(), ERR_NOTREGISTERED(std::string("*")));
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

void IRCServer::updateChannelsClientNickname(int fd, const std::string& newNickname) {
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel& channel = it->second;

        std::map<int, Client*>::const_iterator memberIt = channel.getMembers().find(fd);
        if (memberIt != channel.getMembers().end()) {
            memberIt->second->setNickname(newNickname);
        }
    }
}


/* SIGNALS */
void	IRCServer::handle_signals(int signum)
{
	signal = true;
	(void) signum;
}