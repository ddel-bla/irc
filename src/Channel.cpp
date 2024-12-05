#include "Channel.hpp"

// Constructor
Channel::Channel(const std::string& name)
    : name(name), inviteOnlyFlag(false), topicRestrictedToOps(false), userLimit(0) {}

// Getters and Setters
const std::string& Channel::getName() const {
    return name;
}

void Channel::setChannelKey(const std::string& key) {
    channelKey = key;
}

const std::string& Channel::getChannelKey() const {
    return channelKey;
}

void Channel::removeChannelKey() {
    channelKey.clear();
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

const std::string& Channel::getTopic() const {
    return topic;
}

void Channel::setInviteOnly(bool value) {
    inviteOnlyFlag = value;
}

bool Channel::isInviteOnly() const {
    return inviteOnlyFlag;
}

void Channel::setTopicRestrictedToOps(bool value) {
    topicRestrictedToOps = value;
}

bool Channel::isTopicRestrictedToOps() const {
    return topicRestrictedToOps;
}

void Channel::setUserLimit(size_t limit) {           // Changed to size_t
    userLimit = limit;
}

size_t Channel::getUserLimit() const {               // Changed to size_t
    return userLimit;
}

bool Channel::isUserLimitReached() const {
    return members.size() >= userLimit;               // No longer a sign comparison warning
}

// Methods for managing members
void Channel::addMember(int client_fd, Client* client) {
    members[client_fd] = client;
}

void Channel::removeMember(int client_fd) {
    members.erase(client_fd);
}

bool Channel::isMember(int client_fd) const {
    return members.find(client_fd) != members.end();
}

const std::map<int, Client*>& Channel::getMembers() const {
    return members;
}

// Methods for managing operators
void Channel::addOperator(int client_fd, Client* client) {
    operators[client_fd] = client;
}

void Channel::removeOperator(int client_fd) {
    operators.erase(client_fd);
}

bool Channel::isOperator(int client_fd) const {
    return operators.find(client_fd) != operators.end();
}

// Methods for managing operators
void Channel::addInvited(int client_fd, Client* client) {
    invited[client_fd] = client;
}

void Channel::removeInvited(int client_fd) {
    invited.erase(client_fd);
}

bool Channel::isInvited(int client_fd) const {
    return invited.find(client_fd) != operators.end();
}
