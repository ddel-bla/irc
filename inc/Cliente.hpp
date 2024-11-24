#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>

class Cliente {
private:
    int socket_fd;
    std::string nickname;
    std::string username;
    std::string hostname;

public:
    Cliente(int fd, const std::string& nick, const std::string& user, const std::string& host);
    ~Cliente();

    int getSocketFD() const;
    const std::string& getNickname() const;
    std::string getUsername() const;
    std::string getHostname() const;

    void setNickname(const std::string& nick);
};

#endif // CLIENTE_HPP
