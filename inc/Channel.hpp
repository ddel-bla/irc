#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

class Channel {
    private:
        std::string name;                         // Channel name
        std::map<int, Client*> members;           // Channel members (key: client_fd)
        std::map<int, Client*> operators;         // Channel operators (key: client_fd)
        std::string channelKey;                   // Channel key (if needed)
        std::string topic;                        // Channel topic
        bool inviteOnlyFlag;                      // Changed from 'isInviteOnly' to 'inviteOnlyFlag'
        bool topicRestrictedToOps;                // Indicates if only operators can change the topic
        size_t userLimit;                         // Changed from 'int' to 'size_t'
        
    public:
        // Constructor
        Channel(const std::string& name);

        // Getters and Setters
        const std::string& getName() const;
        void setChannelKey(const std::string& key);
        const std::string& getChannelKey() const;
        void removeChannelKey();
        void setTopic(const std::string& topic);
        const std::string& getTopic() const;
        void setInviteOnly(bool value);
        bool isInviteOnly() const;
        void setTopicRestrictedToOps(bool value);
        bool isTopicRestrictedToOps() const;
        void setUserLimit(size_t limit);           // Changed to size_t
        size_t getUserLimit() const;               // Changed to size_t
        bool isUserLimitReached() const;

        // Methods for managing members and operators
        void addMember(int client_fd, Client* client);
        void removeMember(int client_fd);
        bool isMember(int client_fd) const;
        const std::map<int, Client*>& getMembers() const;

        void addOperator(int client_fd, Client* client);
        void removeOperator(int client_fd);
        bool isOperator(int client_fd) const;
};

#endif
