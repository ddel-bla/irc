#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <vector>
#include <algorithm> 
#include "Utils.hpp"
#include <ctime>
#include "Macros.hpp"

class Client {
	private:
		int 		fd;
		std::string nickname;
		std::string username;
		std::string realname;
		std::string hostname;
		std::string buffer;
		bool		autenticate;
		bool		registred;
		std::time_t connectionTime;
		bool		disconnected;
		std::vector<std::string>	channels;	// CHANNELS PARTICIPATING

	public:
		/* PARAMETRIZED CONSTRUCTOR */
		Client(int fd);
		
		/* DESTRUCTOR */
		~Client();

		/* METHODS */
		void 	clearBuffer(void);
		void 	addChannel(const std::string& channel);
		void	removeChannel(const std::string& channel);
		size_t	getChannelCount() const;
		void 	appendToBuffer(const char* data, size_t length);
		
		/* GETTERS */
		int getFd() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getRealname() const;
		const std::string& getHostname() const;
		const std::string& getBuffer() const;
		bool isAutenticate() const;
		bool isRegistred() const;
		std::time_t getConnectionTime() const;
		bool isDisconnected() const;

		/* SETTERS */
		void setFd(int fd);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setRealname(const std::string& realname);
		void setHostname(const std::string& hostname);
		void setBuffer(const std::string& buffer);
		void setAutenticate(const bool autenticate);
		void setRegistred(const bool registred);
		void setConnectionTime(const std::time_t connectionTime);
		void setDisconnected(const bool disconnected);
		
		/* TO STRING */
		void	toString() const;
		std::string	fdToString();
};

#endif	// CLIENT.HPP
