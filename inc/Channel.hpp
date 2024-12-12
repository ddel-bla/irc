#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"
#include "Macros.hpp"

class Channel {
    private:
        std::string                 name;                   // Channel name
        std::map<int, Client*>      members;                // Channel members (key: client_fd)
        std::map<int, Client*>      operators;              // Channel operators (key: client_fd)
        std::string                 channelKey;             // Channel key (if needed)
        std::map<int, Client*>      invited;                // Channel invited (key: client_fd)
        std::string                 topic;                  // Channel topic
        bool                        inviteOnlyFlag;         // Changed from 'isInviteOnly' to 'inviteOnlyFlag'
        bool                        topicRestricted;        // Indicates if only operators can change the topic
        size_t                      userLimit;              // Changed from 'int' to 'size_t'
        std::vector<std::string>	history;	            // Message history
        std::string	                creationDate;

        
    public:
        /* PARAMETRIZED CONSTRUCTORS */
        Channel(const std::string& name, const std::string& key);
        Channel(const std::string& name);

        /* METHODS */
        void    iMode(bool addMode);
        void    tMode(bool addMode);
        bool    oMode(Client *client, bool addMode);
        bool    kMode(std::string& key, bool addMode);
        bool    lMode(std::string& limit, bool addMode);
        std::string getModes(void);
		void    addMember(int client_fd, Client* client);
		void    removeMember(int client_fd);
		bool    isMember(int client_fd) const;
        void    addOperator(int client_fd, Client* client);
        void    removeOperator(int client_fd);
        bool    isOperator(int client_fd) const;
        void    removeChannelKey();
        bool    isUserLimitReached() const;
        void    addHistoryMsg(const std::string& msg);
        void    addInvited(int client_fd, Client* client);
        void    removeInvited(int client_fd);
        bool    isInvited(int client_fd) const;
        size_t  countMembers() const;
        void    removeMemberfromChannels(int client_fd);

		/* GETTERS */
		const std::string&  getName() const;
		const std::string&  getChannelKey() const;
        const std::string&  getTopic() const;
        bool    isTopicRestricted() const;
        bool    isInviteOnly() const;
        size_t  getUserLimit() const;
		const std::map<int, Client*>&   getMembers() const;
        const std::map<int, Client*>&   getOperators() const;
		const std::vector<std::string>& getHistory() const;
        const std::string& getCreationDate() const;

        /* SETTERS */
        void    setChannelKey(const std::string& key);
        void    setTopic(const std::string& topic);
        void    setInviteOnly(bool value);
        void    setTopicRestricted(bool value);
        void    setUserLimit(size_t limit);

        /* TO STRING */
        void    toString() const;
};

#endif
