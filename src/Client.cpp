#include "Client.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Client::Client(int fd): fd(fd), autenticate(false), registred(false) {
	connectionTime = std::time(NULL);
}

/* DESTRUCTOR */
Client::~Client(void) {
	if (fd >= 0) {
		close(fd);
	}
}

/* METHODS */
void Client::sendMessage(const std::string& message) const {
	std::string formattedMessage = message + "\r\n"; // Add carriage return and newline
	ssize_t bytesSent = send(fd, formattedMessage.c_str(), formattedMessage.size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Error sending message to " << nickname << ": " << strerror(errno) << std::endl;
	}
}

void Client::addChannel(const std::string& channel)
{
	if (std::find(this->channels.begin(), this->channels.end(), channel) == this->channels.end()) {
        this->channels.push_back(channel);
    }
}

/* GETTERS */
int Client::getFd() const {
	return fd;
}

const std::string& Client::getNickname() const {
	return nickname;
}

const std::string& Client::getUsername() const {
	return username;
}

const std::string& Client::getHostname() const {
	return hostname;
}

const std::string& Client::getBuffer() const {
    return buffer;
}

bool Client::isAutenticate() const {
    return autenticate;
}

bool Client::isRegistred() const {
    return registred;
}

std::time_t Client::getConnectionTime() const {
    return connectionTime;
}

/* SETTERS */
void Client::setFd(int fd) {
	this->fd = fd;
}

void Client::setNickname(const std::string& nick) {
	nickname = nick;
}

void Client::setUsername(const std::string& username) {
    this->username = username;
}

void Client::setBuffer(const std::string& buffer) {
    this->buffer = buffer;
}

void Client::setAutenticate(const bool autenticate) {
    this->autenticate = autenticate;
}

void Client::setRegistred(const bool registred) {
    this->registred = registred;
}

void Client::clearBuffer(void)
{
    buffer.clear();
}

