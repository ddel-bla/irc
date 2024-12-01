#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

class Channel {
private:
    std::string name;                         // Name of the channel
    std::map<int, Client*> members;           // Clients in the channel (key: client_fd)

public:
    // Constructor
    Channel(const std::string& name);

    // Get the name of the channel
    const std::string& getName() const;

    // Add a member to the channel
    void addMember(int client_fd, Client* client);

    // Remove a member from the channel
    void removeMember(int client_fd);

    // Check if a client is a member of the channel
    bool isMember(int client_fd) const;

    // Get all members of the channel
    const std::map<int, Client*>& getMembers() const;
};

#endif
