#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"

class Message {
	public:
		Message();

		void sendGlobalMessage(const std::string& message, const std::map<int, Client*>& clients, int client_fd = -1);
		void sendClientMessage(int client_fd, const std::string& message, const std::map<int, Client*>& clients);
		void sendChannelMessage(const std::string& channel, const std::string& message, const std::map<std::string, Channel>& channels, int client_fd = -1);

		void notifyConnection(int client_fd, const std::string& nickname, const std::map<int, Client*>& clients);
		void notifyDisconnection(int client_fd, const std::string& nickname, const std::map<int, Client*>& clients);
};

#endif
