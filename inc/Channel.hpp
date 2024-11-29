#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include "Client.hpp"

class Channel {
	public:
		std::string name;  // Channel name
		std::string topic; // Channel topic
		std::map<int, Client*> members; // Client FD -> Client pointer

		// Constructor
		Channel(const std::string& name);

		// Methods to manage members
		void addMember(int client_fd, Client* client);
		void removeMember(int client_fd);
		bool isMember(int client_fd) const;
		void sendMessage(const std::string& message) const;

		// Get channel information
		size_t memberCount() const;
};

#endif
