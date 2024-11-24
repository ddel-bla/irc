#include "../inc/Cliente.hpp"

Cliente::Cliente(int fd, const std::string& nick, const std::string& user, const std::string& host)
    : socket_fd(fd), nickname(nick), username(user), hostname(host) {}

Cliente::~Cliente() {}

int Cliente::getSocketFD() const {
    return socket_fd;
}

const std::string& Cliente::getNickname() const {
    return nickname;
}

std::string Cliente::getUsername() const {
    return username;
}

std::string Cliente::getHostname() const {
    return hostname;
}

void Cliente::setNickname(const std::string& nick) {
    nickname = nick;
}
