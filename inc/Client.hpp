#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <vector>
#include <cstring>
#include "Utils.hpp"

class Client {
	private:
		int socket_fd;
		std::string nickname;
		std::string username;
		std::string hostname;
		std::string buffer;
		bool	autenticate;
		bool	registred;

	public:
		/* PARAMETRIZED CONSTRUCTOR */
		Client(int fd);
		
		/* DESTRUCTOR */
		~Client();

		/* GETTERS */
		int getSocketFD() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getHostname() const;
		const std::string& getBuffer() const;
		bool isAutenticate() const;
		bool isRegistred() const;

		/* SETTERS */
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setBuffer(const std::string& buffer);
		void setAutenticate(const bool autenticate);
		void setRegistred(const bool registred);

		/* METHODS */
		void clearBuffer(void);
		void sendMessage(const std::string& message) const;
};

#endif
