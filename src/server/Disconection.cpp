#include "IRCServer.hpp"

/* QUIT CMD */
void IRCServer::quit(std::string command, Client& client)
{
	std::vector<std::string> split_command;

	split_command = Utils::splitBySpaces(command);

	logger.info("[QUIT] " + client.fdToString() + " disconnecting...");
	std::string msg = hx_generic_format(command, client);
	message.sendToAll(msg, channels, client.getFd());
	
	removeClient(client.getFd());
}

void IRCServer::removeClient(int client_fd)
{
	close(client_fd);

	// Remove from channels
	removeClientfromChannels(client_fd);

	// Remove from clients
	std::map<int, Client*>::iterator it = clients.find(client_fd);
    
	
    if (it != clients.end()) {
        // SE PUEDE ELIMINAR LO DEJO PORQUE FUNCIONA
		it->second->setNickname("");
		it->second->setUsername("");
		it->second->setConnectionTime(0); 
		it->second->setDisconnected(true);       
        delete it->second;
		clients.erase(it);
    }

	// Remove from fds
	removeFds(client_fd);
}

void IRCServer::removeFds(int fd)
{
	for (size_t i = 0; i < this->fds.size(); i++){
		if (this->fds[i].fd == fd) {
			this->fds.erase(this->fds.begin() + i); 
			return;
		}
	}
}

void IRCServer::removeClientfromChannels(int fd)
{
	for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel& channel = it->second;

		channel.removeMemberfromChannels(fd);	
	}
}