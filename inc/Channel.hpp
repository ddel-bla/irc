#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

class Channel {
    private:
        std::string                 name;                   // Channel name
        std::map<int, Client*>      members;                // Channel members (key: client_fd)
        std::map<int, Client*>      operators;              // Channel operators (key: client_fd)
        std::string                 channelKey;             // Channel key (if needed)
        std::string                 topic;                  // Channel topic
        bool                        inviteOnlyFlag;         // Changed from 'isInviteOnly' to 'inviteOnlyFlag'
        bool                        topicRestrictedToOps;   // Indicates if only operators can change the topic
        size_t                      userLimit;              // Changed from 'int' to 'size_t'
        std::vector<std::string>	history;	            // Message history
        
    public:
        /* PARAMETRIZED CONSTRUCTORS */
        Channel(const std::string& name, const std::string& key);
        Channel(const std::string& name);

        /* METHODS */
		void    addMember(int client_fd, Client* client);
		void    removeMember(int client_fd);
		bool    isMember(int client_fd) const;
        void    addOperator(int client_fd, Client* client);
        void    removeOperator(int client_fd);
        bool    isOperator(int client_fd) const;
        void    removeChannelKey();
        bool    isUserLimitReached() const;
        void    addHistoryMsg(const std::string& msg);

		/* GETTERS */
		const std::string&  getName() const;
		const std::string&  getChannelKey() const;
        const std::string&  getTopic() const;
        bool    isTopicRestrictedToOps() const;
        bool    isInviteOnly() const;
        size_t  getUserLimit() const;
		const std::map<int, Client*>&   getMembers() const;
		const std::vector<std::string>& getHistory() const;

        /* SETTERS */
        void    setChannelKey(const std::string& key);
        void    setTopic(const std::string& topic);
        void    setInviteOnly(bool value);
        void    setTopicRestrictedToOps(bool value);
        void    setUserLimit(size_t limit);
};

#endif
