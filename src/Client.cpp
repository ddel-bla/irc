#include "Client.hpp"
#include <cstring> // For strerror

// Constructor
Client::Client(int fd, const std::string& nick, const std::string& user, const std::string& host)
	: socket_fd(fd), nickname(nick), username(user), hostname(host) {}

// Destructor
Client::~Client() {
	if (socket_fd >= 0) {
		close(socket_fd);
	}
}

// Returns the socket descriptor
int Client::getSocketFD() const {
	return socket_fd;
}

// Returns the nickname
const std::string& Client::getNickname() const {
	return nickname;
}

// Returns the username
const std::string& Client::getUsername() const {
	return username;
}

// Returns the hostname
const std::string& Client::getHostname() const {
	return hostname;
}

// Sets the nickname
void Client::setNickname(const std::string& nick) {
	nickname = nick;
}

// Sends a message to the client
void Client::sendMessage(const std::string& message) const {
	std::string formattedMessage = message + "\r\n"; // Add carriage return and newline
	ssize_t bytesSent = send(socket_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Error sending message to " << nickname << ": " << strerror(errno) << std::endl;
	}
}
