#include "Message.hpp"

// Sends a message to a specific client
void Message::sendToClient(int client_fd, const std::string& message) {
	// Ensures the message is correctly sent to the client
	send(client_fd, message.c_str(), message.length(), 0);
}

// Sends a message to all members of a specific channel
void Message::sendToChannel(const std::string& channel_name, const std::string& message,
							std::map<std::string, Channel>& channels, int exclude_fd) {
	// Looks for the channel in the map
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	if (it == channels.end()) {
        std::cerr << "Channel " << channel_name << " not found." << std::endl;
		return;
	}
    
    it->second.addHistoryMsg(message);
	const std::map<int, Client*>& members = it->second.getMembers();
	for (std::map<int, Client*>::const_iterator member_it = members.begin(); member_it != members.end(); ++member_it) {
		int client_fd = member_it->first;

		// Skip the specified descriptor, if applicable
		if (exclude_fd != -1 && client_fd == exclude_fd)
			continue;

		sendToClient(client_fd, message);
	}
}

// Sends a message to all clients that are members of any channel in which the given client (identified by client_fd) is also a member.
void Message::sendToAll(const std::string& message,
                        const std::map<std::string, Channel>& channels, int client_fd) {
    // Iterate through all channels
    for (std::map<std::string, Channel>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        const Channel& channel = it->second;

        // Check if the given client_fd is a member of the channel
        if (channel.isMember(client_fd)) {
            const std::map<int, Client*>& members = channel.getMembers();

            // Send the message to all other members of this channel
            for (std::map<int, Client*>::const_iterator member_it = members.begin(); member_it != members.end(); ++member_it) {
                int target_fd = member_it->first;

                // Skip sending to the client_fd itself
                if (target_fd == client_fd)
                    continue;

                sendToClient(target_fd, message);
            }
        }
    }
}
