#include "Channel.hpp"

// Constructor
Channel::Channel(const std::string& name)
    : name(name) {}

// Get the name of the channel
const std::string& Channel::getName() const {
    return name;
}

// Add a member to the channel
void Channel::addMember(int client_fd, Client* client) {
    members[client_fd] = client;
}

// Remove a member from the channel
void Channel::removeMember(int client_fd) {
    members.erase(client_fd);
}

// Check if a client is a member of the channel
bool Channel::isMember(int client_fd) const {
    return members.find(client_fd) != members.end();
}

// Get all members of the channel
const std::map<int, Client*>& Channel::getMembers() const {
    return members;
}
