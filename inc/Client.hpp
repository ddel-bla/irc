#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <cerrno>

class Client {
	private:
		int socket_fd;
		std::string nickname;
		std::string username;
		std::string hostname;

	public:
		Client(int fd, const std::string& nick, const std::string& user, const std::string& host);
		~Client();

		// Getter methods
		int getSocketFD() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getHostname() const;

		// Setter methods
		void setNickname(const std::string& nick);

		// Send a message to the client
		void sendMessage(const std::string& message) const;
};

#endif
