#include "Channel.hpp"

// Constructor
Channel::Channel(const std::string& name) : name(name), topic("") {}

// Adds a member to the channel
void Channel::addMember(int client_fd, Client* client) {
	if (members.find(client_fd) == members.end()) {
		members[client_fd] = client;
		std::cout << "Client with FD " << client_fd << " added to channel " << name << "." << std::endl;
	} else {
		std::cerr << "Client with FD " << client_fd << " is already a member of channel " << name << "." << std::endl;
	}
}

// Removes a member from the channel
void Channel::removeMember(int client_fd) {
	if (members.erase(client_fd) > 0) {
		std::cout << "Client with FD " << client_fd << " removed from channel " << name << "." << std::endl;
	} else {
		std::cerr << "Client with FD " << client_fd << " is not a member of channel " << name << "." << std::endl;
	}
}

// Checks if a client is a member of the channel
bool Channel::isMember(int client_fd) const {
	return members.find(client_fd) != members.end();
}

// Sends a message to all members of the channel
void Channel::sendMessage(const std::string& message) const {
	for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
		Client* client = it->second; // Get the client pointer
		if (client) {
			client->sendMessage(message); // Call Client's sendMessage method
		} else {
			std::cerr << "Error: Null client in channel " << name << "." << std::endl;
		}
	}
}

// Gets the number of members in the channel
size_t Channel::memberCount() const {
	return members.size();
}
