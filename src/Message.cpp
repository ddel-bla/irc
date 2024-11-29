#include "Message.hpp"
#include <iostream>
#include <sstream> // For std::ostringstream

// Constructor
Message::Message() {}

// Sends a message to all connected clients except the specified client_fd
void Message::sendGlobalMessage(const std::string& message, const std::map<int, Client*>& clients, int client_fd) {
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->first != client_fd && it->second) {
            it->second->sendMessage(message);
        }
    }
}

// Sends a message to a specific client
void Message::sendClientMessage(int client_fd, const std::string& message, const std::map<int, Client*>& clients) {
    std::map<int, Client*>::const_iterator it = clients.find(client_fd);
    if (it != clients.end() && it->second) {
        it->second->sendMessage(message);
    } else {
        std::cerr << "Error: Client not found for FD " << client_fd << std::endl;
    }
}

// Sends a message to all members of a channel except the specified client_fd
void Message::sendChannelMessage(const std::string& channel, const std::string& message, const std::map<std::string, Channel>& channels, int client_fd) {
    std::map<std::string, Channel>::const_iterator it = channels.find(channel);
    if (it != channels.end()) {
        const std::map<int, Client*>& members = it->second.members;
        for (std::map<int, Client*>::const_iterator member = members.begin(); member != members.end(); ++member) {
            if (member->first != client_fd && member->second) {
                member->second->sendMessage(message);
            }
        }
    } else {
        std::cerr << "Error: Channel not found: " << channel << std::endl;
    }
}

// Notifies about a client's connection
void Message::notifyConnection(int client_fd, const std::string& nickname, const std::map<int, Client*>& clients) {
    std::string message = "User " + nickname + " has connected.";
    sendGlobalMessage(message, clients, client_fd);
}

// Notifies about a client's disconnection
void Message::notifyDisconnection(int client_fd, const std::string& nickname, const std::map<int, Client*>& clients) {
    std::string message = "User " + nickname + " has disconnected.";
    sendGlobalMessage(message, clients, client_fd);
}
