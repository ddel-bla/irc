#include "Message.hpp"

// Send a message to a single client
void Message::sendToClient(int client_fd, const std::string& message) {
	if (send(client_fd, message.c_str(), message.size(), 0) < 0)
		std::cerr << "Error sending message to client " << client_fd << std::endl;
}

// Send a message to all members of a specific channel
void Message::sendToChannel(const std::string& channel_name, const std::string& message,
							int exclude_fd, const std::map<std::string, Channel>& channels) {
	std::map<std::string, Channel>::const_iterator it = channels.find(channel_name);
	if (it != channels.end()) {
		const Channel& channel = it->second;
		const std::map<int, Client*>& members = channel.getMembers();
		for (std::map<int, Client*>::const_iterator member = members.begin(); member != members.end(); ++member) {
			if (member->first != exclude_fd)
				sendToClient(member->first, message);
		}
	} else {
		std::cerr << "Channel " << channel_name << " not found." << std::endl;
	}
}

// Send a message to all clients across all channels
void Message::sendToAll(const std::string& message, int exclude_fd,
						const std::map<std::string, Channel>& channels) {
	for (std::map<std::string, Channel>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
		sendToChannel(it->first, message, exclude_fd, channels);
	}
}

// Notify an event (connection, disconnection, etc.) to all clients across channels
void Message::notifyEvent(const std::string& event_type, const std::string& nickname, int exclude_fd,
						const std::map<std::string, Channel>& channels) {
	std::string event_message = "[" + event_type + "] " + nickname + " has joined/left.";
	sendToAll(event_message, exclude_fd, channels);
}
