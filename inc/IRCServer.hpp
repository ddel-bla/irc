#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <cstring>    // For memset and strerror
#include <unistd.h>   // For close
#include <fcntl.h>    // For fcntl and O_NONBLOCK
#include <netinet/in.h> // For sockaddr_in, INADDR_ANY, htons
#include <arpa/inet.h>  // For network address functions
#include <poll.h>       // For poll
#include <cstdlib> // For atoi
#include <iostream>
#include "Client.hpp"
#include "IRCServer.hpp"
#include "Message.hpp"
#include "Macros.hpp"

class IRCServer
{
private:
		int	port;
		std::string password;
		int server_fd;
		std::vector<struct pollfd> fds;
		std::map<int, Client*> clients; // FD to Client object mapping
		std::map<std::string, Channel> channels; // Map to manage channels
		Message message; // New attribute to handle events

		void acceptClient();
		void processClient(int client_fd);
		void removeClient(int client_fd);

		void receiveData(int fd);
		void process_command(std::string command, int fd);
		void quit(std::string command, int fd);

		/* REGISTRATION METHODS */
		void	authenticate(std::string command, Client& client);
		void	registerNickname(std::string command, Client& client);
		void	registerUsername(std::string command, Client& client);
		bool	isValidNickname(const std::string nickname);
		bool	isNicknameTaken(const std::string nickname);

public:
		IRCServer(int port, const std::string& password);
		bool startServer();
		void run();
		std::vector<std::string> getClientChannels(int client_fd) const;
};

#endif
