#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include "Channel.hpp"

class Message {
public:
    void sendToAll(const std::string& message, int exclude_fd,
                   const std::map<std::string, Channel>& channels);

    void sendToChannel(const std::string& channel_name, const std::string& message,
                       int exclude_fd, const std::map<std::string, Channel>& channels);

    void notifyEvent(const std::string& event_type, const std::string& nickname, int exclude_fd,
                     const std::map<std::string, Channel>& channels);
};

#endif // MESSAGE_HPP
