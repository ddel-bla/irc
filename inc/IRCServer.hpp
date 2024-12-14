#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <cstring>    // For memset and strerror
#include <unistd.h>   // For close
#include <fcntl.h>    // For fcntl and O_NONBLOCK
#include <netinet/in.h> // For sockaddr_in, INADDR_ANY, htons
#include <arpa/inet.h>  // For network address functions
#include <poll.h>       // For poll
#include <cstdlib> // For atoi
#include <iostream>
#include <csignal>
#include "Client.hpp"
#include "IRCServer.hpp"
#include "Message.hpp"
#include "Macros.hpp"
#include "Logger.hpp"
#include "TrivialBot.hpp"

enum Command {
	PASS, 
	USER,
	NICK,
	QUIT,
	PRIVMSG,
	JOIN,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	WHO,
	PART,
	TRIVIAL,
    START,
    ANSWER,
    T_QUIT,
    HELP,
    UNKNOWN 
};

class IRCServer
{
private:
		/* ATRIBUTES */
		std::string	version;
		std::string	servername;
		std::string	creationDate;
		int			port;
		std::string	password;
		int			server_fd;
		std::vector<struct pollfd>		fds;
		std::map<int, Client*>			clients; 	// FD to Client object mapping
		std::map<std::string, Channel>	channels; 	// Map to manage channels
		Logger 							logger;		// Logger
		Message 						message; 	// New attribute to handle events
		std::map<std::string, Command> 	commandMap; 	// Switch cmd map
		TrivialBot						trivialBot;

		/* GLOBAL VARIABLES */
		static bool signal;
		
		/* METHODS */
		void acceptClient();
		void removeClient(int client_fd);
		void receiveData(int fd);
		void process_command(std::string command, int fd);
		void quit(std::string command, Client& client);
		void removeFds(int fd);
		void removeClientfromChannels(int fd);
		std::map<std::string, Command> createCommandMap();
		Command commandToInt(const std::string& command);

		/* REGISTRATION METHODS */
		void 	checkRegistrationTimeout(void);
		void	authenticate(std::string command, Client& client);
		void	registerNickname(std::string command, Client& client);
		void	registerUsername(std::string command, Client& client);
		void	registerRealname(std::string& command, Client& Client);
		bool	isValidNickname(const std::string nickname);
		bool	isNicknameTaken(const std::string nickname);
		void	updateChannelsClientNickname(int fd, const std::string& newNickname);
		void	sendwelcomeMessage(int fd, const std::string& nickname);
		void	sendRplISupport(int fd, const std::string& nickname);

		/* HEXCLIENT MSG FORMAT */
		std::string	hx_generic_format(const std::string& command, Client& sender);
		std::string	hx_join_format(const std::string& command, Client& sender, bool member_joined);
		std::string	hx_quit_format(const std::string& channel, Client& sender, const std::string& kicked_user, std::string& reason);
		std::string hx_mode_format(const std::string& channel, Client& sender, bool addMode, char modeChar, std::string& param);
		std::string hx_who_format(Channel& channelName, Client& sender, const std::map<int, Client*>& members);
		std::string hx_part_format(const std::string& channel_name, Client& sender);

		/* PRIVMSG */
		void	privMsg(const std::string& command, Client& clien);
		int		findFdByNickname(const std::string& nickname);
		bool	existsChannelByName(const std::string& name);

		/* JOIN */
		void	join(const std::string& command, Client& client);
		bool	isChannelNameValid(const std::string& channelName);
		void	showChannelHistory(const std::vector<std::string> history, int fd);

		/* ---- CHANNEL CMDS ---- */
		/* KICK */
		void		kick(const std::string& command, Client& client);
		std::string	getKickReason(const std::string& command, int command_len);
		
		/* INVITE */
		void	invite(const std::string& command, Client& client);

		/* TOPIC */
		void	topic(const std::string& command, Client& client);
		
		/* MODE */
		void	mode(const std::string& command, Client& client);
		void	setModes(Channel& channel, std::string& modes, std::vector<std::string> params, Client& sender);

		/* WHO */
		void	who(const std::string& command, Client& client);

		/* PART */
		void	part(const std::string& command, Client& client);

		/* BOT */
		void	trivial(std::string& commmand, Client& client);
public:
		/* PARAMETRIZED CONSTRUCTOR*/
		IRCServer(int port, const std::string& password);

		~IRCServer();
		
		/* MAIN METHODS*/
		bool startServer();
		void run();
		
		/* SIGNAL HANDLING*/
		static void	handle_signals(int signal);

		/* TO STRING */
		void	toString() const;
};

#endif	//IRCServer.hpp
