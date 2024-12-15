#include "Client.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Client::Client(int fd): fd(fd), autenticate(false), registred(false), disconnected(false) {
	connectionTime = std::time(NULL);
}

/* DESTRUCTOR */
Client::~Client(void) {
	if (fd >= 0) {
		close(fd);
	}
}

/* METHODS */

void Client::addChannel(const std::string& channel)
{
	if (std::find(this->channels.begin(), this->channels.end(), channel) == this->channels.end()) {
        this->channels.push_back(channel);
    }
}

void Client::removeChannel(const std::string& channel)
{
    std::vector<std::string>::iterator it = std::find(this->channels.begin(), this->channels.end(), channel);
    if (it != this->channels.end()) {
        this->channels.erase(it);
    }
}

void Client::clearBuffer(void)
{
    buffer.clear();
}

void Client::appendToBuffer(const char* data, size_t length) {
    buffer.append(data, length);
}

size_t Client::getChannelCount() const {
    return channels.size();
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

const std::string& Client::getRealname() const {
    return realname;
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

bool Client::isDisconnected() const {
    return disconnected;
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

void Client::setHostname(const std::string& hostname) {
	this->hostname = hostname;
}

void Client::setRealname(const std::string& realname) {
    this->realname = realname;
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

void Client::setConnectionTime(const std::time_t connectionTime) {
	this->connectionTime = connectionTime;
}

void Client::setDisconnected(const bool disconnected) {
    this->disconnected = disconnected;
}

/* TO STRING */
std::string Client::fdToString()
{
	return "Client fd:" + Utils::intToString(fd);
}

void Client::toString() const {
    std::ostringstream output;

    // Header
    output << BOLD << GREEN << "> Client Info (fd: " << fd << ")" << RESET << "\n";
    output << CYAN << "Nickname: " << RESET << (nickname.empty() ? "(No nickname)" : nickname) << "\n";
    output << CYAN << "Username: " << RESET << (username.empty() ? "(No username)" : username) << "\n";
    output << CYAN << "Hostname: " << RESET << (hostname.empty() ? "(No hostname)" : hostname) << "\n";

    // States
    output << YELLOW << "Authenticated: " << RESET << (autenticate ? "Yes" : "No") << "\n";
    output << YELLOW << "Registered: " << RESET << (registred ? "Yes" : "No") << "\n";

    // Connectio time
    char timeBuffer[64];
    if (std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&connectionTime))) {
        output << BLUE << "Connection Time: " << RESET << timeBuffer << "\n";
    } else {
        output << BLUE << "Connection Time: " << RESET << "(Unknown)\n";
    }

    // Channels
    output << RED << "Channels (" << channels.size() << "):" << RESET << "\n";
    for (std::vector<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        output << "  - " << *it << "\n";
    }

    // Buffer
    output << YELLOW << "Buffer: " << RESET << (buffer.empty() ? "(Empty)" : buffer) << "\n";

    // Print all
    std::cout << output.str();
}