#include "IRCServer.hpp"

bool IRCServer::signal = false;

/* PARAMETRIZED CONSTRUCTOR */
IRCServer::IRCServer(int port, const std::string& password): version(VERSION), servername(SERVERNAME), port(port), password(password), server_fd(-1), logger("application.log", true), trivialBot(F_QUESTIONS) {
	this->commandMap = createCommandMap();
	this->creationDate = Utils::getCurrentTimeISO8601();
}

/* DESTRUCTOR */
IRCServer::~IRCServer()
{
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete it->second; 
    }
    clients.clear();
}

/* METHODS */

bool IRCServer::startServer()
{
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		logger.error(std::string("Error creating socket: ") + strerror(errno));
		return false;
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		logger.error(std::string("Error in setsockopt: ") + strerror(errno));
		close(server_fd);
		return false;
	}

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0) {
		logger.error(std::string("Error setting non-blocking mode: ") + strerror(errno));
		close(server_fd);
		return false;
	}

	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		logger.error(std::string("Error in bind: ") + strerror(errno));
		close(server_fd);
		return false;
	}

	if (listen(server_fd, 5) < 0) {
		logger.error(std::string("Error in listen: ") + strerror(errno));
		close(server_fd);
		return false;
	}
	
	logger.info("Server started on port " + Utils::intToString(port));
	return true;
}

void IRCServer::run()
{
	struct	pollfd	pfd;
	
	pfd.fd = server_fd;
	pfd.events = POLLIN;
	fds.push_back(pfd);
	while (!IRCServer::signal)
	{
		int ret = poll(fds.data(), fds.size(), 1000); // Timeout 1 sec
		if (ret < 0  && !IRCServer::signal) {
			logger.error(std::string("Error in poll: ") + strerror(errno));
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

void IRCServer::acceptClient()
{
    sockaddr_in	client_address;
	std::memset(&client_address, 0, sizeof(client_address));
    socklen_t	address_length = sizeof(client_address);

    int	client_fd = accept(server_fd, (struct sockaddr*)&client_address, &address_length);

    if (client_fd < 0) {
        if (errno != EWOULDBLOCK) {
			logger.error(std::string("Error accepting client: ") + strerror(errno));
        }
        return;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
		logger.error(std::string("Error setting client to non-blocking mode: ") + strerror(errno));
        close(client_fd);
        return;
    }

    Client* new_client = new Client(client_fd);
    clients[client_fd] = new_client;

    struct pollfd new_client_fd;
    new_client_fd.fd = client_fd;
    new_client_fd.events = POLLIN;
	new_client_fd.revents = 0;
	new_client->setHostname(inet_ntoa(client_address.sin_addr));
    fds.push_back(new_client_fd);
	(void) new_client_fd;

    logger.info("New client connected in fd: " + Utils::intToString(client_fd) + "and ip: " + new_client->getHostname());
}

void IRCServer::receiveData(int fd)
{
    char 		buffer[512];
    size_t 		bytes;
    Client* 	client = clients[fd];
    std::vector<std::string> commands;

    memset(buffer, 0, sizeof(buffer));
    bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0)
    {
        if (bytes == 0) {
            std::string msg = hx_generic_format(QUIT_DEFAULT, *client);
            logger.info(client->fdToString() + " disconnecting... (" + msg + ")");
            message.sendToAll(msg, channels, client->getFd());
        } 
        else
            logger.error(std::string("Error receiving data: ") + strerror(errno));
        removeClient(fd);
        return;
    }

    if (client)
    {
        client->appendToBuffer(buffer, bytes);
        logger.info(client->fdToString() + " sent: " + client->getBuffer());

        if (client->getBuffer().find_first_of(CRLF) == std::string::npos)
		{
			return;
		}

        // SPLIT BY '\r\n'
        commands = Utils::split(client->getBuffer(), CRLF);
        
        // PROCESS EACH COMMAND
        for (size_t i = 0; i < commands.size(); i++)
			if (!commands[i].empty())
            	process_command(commands[i], fd);
        
        client->clearBuffer();
    }
}

/* SIGNALS */
void	IRCServer::handle_signals(int signum)
{
	signal = true;
	(void) signum;
}

/* SWITCH CMD MAP */
std::map<std::string, Command> IRCServer::createCommandMap()
{
    commandMap["START"] = START;
    commandMap["ANSWER"] = ANSWER;
    commandMap["QUIT"] = T_QUIT;
    commandMap["HELP"] = HELP;
    return commandMap;
}

Command IRCServer::commandToInt(const std::string& command) {
    std::map<std::string, Command>::const_iterator it = commandMap.find(command);
    if (it != commandMap.end()) {
        return it->second;
    }
    return UNKNOWN;
}

/* TO STRING */
void	IRCServer::toString() const
{
	std::ostringstream output;

	// Main information
	output << BOLD << GREEN << "IRC Server Info" << RESET << "\n";
	output << CYAN << "Port: " << RESET << port << "\n";
	output << CYAN << "Password: " << RESET << (password.empty() ? "(No password set)" : password) << "\n";
	output << CYAN << "Server FD: " << RESET << server_fd << "\n";

	// Conected clients
	output << BLUE << "Clients (" << clients.size() << "):" << RESET << "\n";
	for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
		output << "  - FD: " << it->first << " | Nickname: " << it->second->getNickname() << "\n";
		it->second->toString();
	}

	// Channels
	output << RED << "Channels (" << channels.size() << "):" << RESET << "\n";
	for (std::map<std::string, Channel>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		output << "  - Channel: " << it->first << "\n";
		it->second.toString();  // Llamamos al toString del canal para obtener detalles
	}

	// Fds info
	output << YELLOW << "Poll FDs (" << fds.size() << "):" << RESET << "\n";
	for (size_t i = 0; i < fds.size(); ++i) {
		output << "  - FD: " << fds[i].fd << " | Events: " << fds[i].events << "\n";
	}

	// Print all
	std::cout << output.str();
}