#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include "Client.hpp"

class IRCServer {
private:
	int server_socket;
	std::vector<pollfd> poll_fds;
	std::map<int, Client> clients; // Mapa de socket a Cliente

public:
	IRCServer();
	~IRCServer();

	bool start();
	void run();
	void accept_client();
	void handle_client(int client_socket);
	void close_client(int client_socket);
};

#endif
