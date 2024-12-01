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
					int exclude_fd, const std::map<std::string, Channel>& channels);

	void sendToAll(const std::string& message, int exclude_fd,
				const std::map<std::string, Channel>& channels);

	void notifyEvent(const std::string& event_type, const std::string& nickname, int exclude_fd,
					const std::map<std::string, Channel>& channels);
};

#endif // MESSAGE_HPP
