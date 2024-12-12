#include "Channel.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Channel::Channel(const std::string& name, const std::string& key): name(name), channelKey(key), inviteOnlyFlag(false), topicRestricted(false), userLimit(0) {
    this->creationDate = Utils::getCurrentTimeISO8601();
}

Channel::Channel(const std::string& name): name(name), inviteOnlyFlag(false), topicRestricted(false), userLimit(0) {
    this->creationDate = Utils::getCurrentTimeISO8601();
}

/* METHODS */

std::string Channel::getModes(void)
{
    std::string modes;

    modes = "+";
    if (isInviteOnly())
        modes += "i";
    if (isTopicRestricted())
        modes += "t";
    if (getUserLimit() > 0)
        modes += "l";
    if (!getChannelKey().empty())
        modes += "k";
    
    // ONLY +
    if (modes.size() == 1)
        return "";
    
    if (getUserLimit() > 0)
        modes += " " + Utils::intToString(getUserLimit());
    
    return modes;
}

void Channel::iMode(bool addMode)
{
    inviteOnlyFlag = addMode;
}

void Channel::tMode(bool addMode)
{
    topicRestricted = addMode;
}

bool Channel::oMode(Client *client, bool addMode)
{
    // Is operator to remove
    if (isOperator(client->getFd()) && !addMode)
    {
        removeOperator(client->getFd());
        return true;
    }
    else if (addMode)
    {
        addOperator(client->getFd(), client);
        return true;
    }

    return false;
}

bool Channel::kMode(std::string& key, bool addMode)
{   
    if (addMode && key.empty())
        return false;
    if (addMode)
        channelKey = key; // Set key
    else
        channelKey.clear(); // Remove key
    return true;
}

bool Channel::lMode(std::string& param, bool addMode)
{
    if (addMode) {
        for (size_t j = 0; j < param.length(); ++j) {
            if (!std::isdigit(param[j])) {
                return false;
            }
        }

        size_t limit = std::atoi(param.c_str());
        userLimit = limit;
    } else {
        userLimit = 0;
    }

    return true;
}

void Channel::removeMemberfromChannels(int client_fd)
{
    if (isMember(client_fd))
        removeMember(client_fd);
    if (isOperator(client_fd))
        removeOperator(client_fd);
    if (isInvited(client_fd))
        removeInvited(client_fd);
}

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

// Methods for managing operators
void Channel::addInvited(int client_fd, Client* client) {
    invited[client_fd] = client;
}

void Channel::removeInvited(int client_fd) {
    invited.erase(client_fd);
}

bool Channel::isInvited(int client_fd) const {
    return invited.find(client_fd) != invited.end();
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

size_t Channel::countMembers() const
{
    return members.size();
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

bool Channel::isTopicRestricted() const {
    return topicRestricted;
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

const std::string& Channel::getCreationDate() const {
    return creationDate;
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

void Channel::setTopicRestricted(bool value) {
    topicRestricted = value;
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
    output << YELLOW << "Topic Restricted to Ops: " << RESET << (topicRestricted ? "Yes" : "No") << "\n";
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
    
    // Invited
    output << BLUE << "Invited (" << invited.size() << "):" << RESET << "\n";
    for (std::map<int, Client*>::const_iterator it = invited.begin(); it != invited.end(); ++it) {
        output << "  - " << it->second->getNickname() << " (fd: " << it->first << ")\n";
    }
    
    // History
    // output << CYAN << "Message History (" << history.size() << " messages):" << RESET << "\n";
    // for (std::vector<std::string>::const_iterator it = history.begin(); it != history.end(); ++it) {
    //     output << "  * " << *it << "\n";
    // }

    // Print all
    std::cout << output.str();
}
