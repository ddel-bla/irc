#include "Channel.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Channel::Channel(const std::string& name, const std::string& key): name(name), channelKey(key), inviteOnlyFlag(false), topicRestrictedToOps(false), userLimit(0) {}

Channel::Channel(const std::string& name): name(name), inviteOnlyFlag(false), topicRestrictedToOps(false), userLimit(0) {}

/* METHODS */
void Channel::addMember(int client_fd, Client *client) {
    members[client_fd] = client;
}

void Channel::removeMember(int client_fd) {
    members.erase(client_fd);
}

bool Channel::isMember(int client_fd) const {
    return members.find(client_fd) != members.end();
}

void Channel::addOperator(int client_fd, Client* client) {
    operators[client_fd] = client;
}

void Channel::removeOperator(int client_fd) {
    operators.erase(client_fd);
}

bool Channel::isOperator(int client_fd) const {
    return operators.find(client_fd) != operators.end();
}

void Channel::removeChannelKey() {
    channelKey.clear();
}

bool Channel::isUserLimitReached() const {
    return members.size() >= userLimit;
}

void Channel::addHistoryMsg(const std::string& msg)
{
    history.push_back(msg);
}

/* GETTERS */
const std::string& Channel::getName() const {
    return name;
}

const std::string& Channel::getChannelKey() const {
    return channelKey;
}

const std::string& Channel::getTopic() const {
    return topic;
}

bool Channel::isTopicRestrictedToOps() const {
    return topicRestrictedToOps;
}

bool Channel::isInviteOnly() const {
    return inviteOnlyFlag;
}

size_t Channel::getUserLimit() const {
    return userLimit;
}

const std::map<int, Client*>& Channel::getMembers() const {
    return members;
}

const std::map<int, Client*>& Channel::getOperators() const {
    return operators;
}

const std::vector<std::string>& Channel::getHistory() const {
    return history;
}

/* SETTERS */
void Channel::setChannelKey(const std::string& key) {
    channelKey = key;
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

void Channel::setInviteOnly(bool value) {
    inviteOnlyFlag = value;
}

void Channel::setTopicRestrictedToOps(bool value) {
    topicRestrictedToOps = value;
}

void Channel::setUserLimit(size_t limit) {
    userLimit = limit;
}

/* TO STRING */
void Channel::toString() const {
    std::ostringstream output;

    // Header
    output << BOLD << GREEN << "> Channel: " << name << RESET << "\n";
    output << CYAN << "Topic: " << RESET << (topic.empty() ? "(No topic set)" : topic) << "\n";
    output << YELLOW << "Invite Only: " << RESET << (inviteOnlyFlag ? "Yes" : "No") << "\n";
    output << YELLOW << "Topic Restricted to Ops: " << RESET << (topicRestrictedToOps ? "Yes" : "No") << "\n";
    output << YELLOW << "User Limit: " << RESET << (userLimit == 0 ? "Unlimited" : Utils::intToString(userLimit)) << "\n";
    output << RED << "Channel Key: " << RESET << (channelKey.empty() ? "(No key set)" : channelKey) << "\n";

    // Members
    output << BLUE << "Members (" << members.size() << "):" << RESET << "\n";
    for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        output << "  - " << it->second->getNickname() << " (fd: " << it->first << ")\n";
    }

    // Operators
    output << BLUE << "Operators (" << operators.size() << "):" << RESET << "\n";
    for (std::map<int, Client*>::const_iterator it = operators.begin(); it != operators.end(); ++it) {
        output << "  - " << it->second->getNickname() << " (fd: " << it->first << ")\n";
    }

    // History
    output << CYAN << "Message History (" << history.size() << " messages):" << RESET << "\n";
    for (std::vector<std::string>::const_iterator it = history.begin(); it != history.end(); ++it) {
        output << "  * " << *it << "\n";
    }

    // Print all
    std::cout << output.str();
}

