#include "Message.hpp"
#include <iostream>

// Send a message to all clients across all channels
void Message::sendToAll(const std::string& message, int exclude_fd,
                        const std::map<std::string, Channel>& channels) {
    for (std::map<std::string, Channel>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        const Channel& channel = it->second;
        const std::map<int, Client*>& members = channel.getMembers();
        for (std::map<int, Client*>::const_iterator member = members.begin(); member != members.end(); ++member) {
            if (member->first != exclude_fd)
                send(member->first, message.c_str(), message.size(), 0);
        }
    }
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
                send(member->first, message.c_str(), message.size(), 0);
        }
    } else {
        std::cerr << "Channel " << channel_name << " not found." << std::endl;
    }
}

// Notify an event (connection, disconnection, etc.) to all clients across channels
void Message::notifyEvent(const std::string& event_type, const std::string& nickname, int exclude_fd,
                          const std::map<std::string, Channel>& channels) {
    std::string event_message = "[" + event_type + "] " + nickname + " has joined/left.";
    sendToAll(event_message, exclude_fd, channels);
}
