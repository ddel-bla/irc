#include "IRCServer.hpp"

/* QUIT CMD */
void IRCServer::quit(std::string command, Client& client)
{
	std::vector<std::string> split_command;

	split_command = Utils::splitBySpaces(command);

	logger.info("[QUIT] " + client.fdToString() + " disconnecting...");
	std::string msg = hx_generic_format(command, client);
	message.sendToAll(msg, channels, client.getFd());
	
	close(client.getFd());
	removeFds(client.getFd());
	removeClient(client.getFd());
}

void IRCServer::removeClient(int client_fd)
{
	close(client_fd);

	std::map<int, Client*>::iterator it = clients.find(client_fd);
    
    if (it != clients.end()) {
        // Liberar la memoria del cliente
        it->second->setNickname("");
		it->second->setUsername("");
		it->second->setConnectionTime(0); 
		it->second->setDisconnected(true);       
        // Eliminar la entrada del mapa
        //clients.erase(it);
    }

	removeFds(client_fd);
	// for (size_t i = 0; i < fds.size(); ++i) {
	// 	if (fds[i].fd == client_fd) {
	// 		fds.erase(fds.begin() + i);
	// 		break;
	// 	}
	// }
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