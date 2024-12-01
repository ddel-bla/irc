#include "Client.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Client::Client(int fd): socket_fd(fd), autenticate(false), registred(false) {}

/* DESTRUCTOR */
Client::~Client(void) {
	if (socket_fd >= 0) {
		close(socket_fd);
	}
}


/* GETTERS & SETTERS */

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

// Get buffer
const std::string& Client::getBuffer() const {
    return buffer;
}

// Is Autenticate
bool Client::isAutenticate() const {
    return autenticate;
}

// Is Registred
bool Client::isRegistred() const {
    return registred;
}

// Sets the nickname
void Client::setNickname(const std::string& nick) {
	nickname = nick;
}

// Modifica el username
void Client::setUsername(const std::string& username) {
    this->username = username;
}

// Set buffer
void Client::setBuffer(const std::string& buffer) {
    this->buffer = buffer;
}

// Set Autenticate
void Client::setAutenticate(const bool autenticate) {
    this->autenticate = autenticate;
}

// Set Registred
void Client::setRegistred(const bool registred) {
    this->registred = registred;
}

void Client::clearBuffer(void)
{
    buffer.clear();
}

// Sends a message to the client
void Client::sendMessage(const std::string& message) const {
	std::string formattedMessage = message + "\r\n"; // Add carriage return and newline
	ssize_t bytesSent = send(socket_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Error sending message to " << nickname << ": " << strerror(errno) << std::endl;
	}
}
