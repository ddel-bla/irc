#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <iostream>
#include <sys/socket.h> // Para `send`
#include "Channel.hpp"
#include "Client.hpp"

class Message {
	public:
		void sendToClient(int client_fd, const std::string& message);


		void sendToChannel(const std::string& channel_name, const std::string& message,
						const std::map<std::string, Channel>& channels, int exclude_fd = -1);

		void sendToAll(const std::string& message,
					const std::map<std::string, Channel>& channels, int exclude_fd = -1);

};

#endif // MESSAGE_HPP
