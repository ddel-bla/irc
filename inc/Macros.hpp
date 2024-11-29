#ifndef MACROS_HPP
# define MACROS_HPP
#define RED		"\033[0;91m"
#define GREEN   "\033[1;92m"
#define YELLOW	"\033[1;93m"
#define BLUE	"\033[0;94m"
#define MAGENTA	"\033[0;95m"
#define CYAN	"\033[0;96m"
#define WHITE	"\033[0;97m"
#define RESET	"\033[0m"
#define CRLF "\r\n"
#define RPL_CONNECTED(nick)							(": 001 " + nick + " : Welcome to the IRC server!" + CRLF)
#define RPL_UMODEIS(host, channel, mode, user)  	":" + host + " MODE " + channel + " " + mode + " " + user + CRLF
#define RPL_CREATIONTIME(nick, channel, time) 		": 329 " + nick + " #" + channel + " " + time + CRLF
#define RPL_CHANNELMODES(nick, channel, modes) 		": 324 " + nick + " #" + channel + " " + modes + CRLF
#define RPL_CHANGEMODE(host, channel, mode, args) 	(":" + host + " MODE #" + channel + " " + mode + " " + args + CRLF)
#define RPL_clientNickCHANGE(oldnick, nick) 		(":" + oldnick + " NICK " + nick + CRLF)
#define RPL_JOINMSG(host, ipaddress, channel) 		(":" + host + "@" + ipaddress + " JOIN #" + channel + CRLF)
#define RPL_NAMREPLY(nick, channel, clientslist) 	(": 353 " + nick + " @ #" + channel + " :" + clientslist + CRLF)
#define RPL_ENDOFNAMES(nick, channel) 				(": 366 " + nick + " #" + channel + " :END of /NAMES list" + CRLF)
#define RPL_TOPICIS(nick, channel, topic) 			(": 332 " + nick + " #" +channel + " :" + topic + CRLF)
///////// ERRORS ////////////////
#define ERR_NEEDMODEPARM(channel, mode) 			(": 696 #" + channel + " * You must specify a parameter for the key mode. " + mode + CRLF)
#define ERR_INVALIDMODEPARM(channel, mode) 			": 696 #" + channel + " Invalid mode parameter. " + mode + CRLF
#define ERR_KEYSET(channel) 						": 467 #" + channel + " Channel key already set. " + CRLF
#define ERR_UNKNOWNMODE(nick, channel, mode)	 	": 472 " + nick + " #" + channel + " " + mode + " :is not a recognised channel mode" + CRLF
#define ERR_NOTENOUGHPARAM(nick) 					(": 461 " + nick + " :Not enough parameters." + CRLF)
#define ERR_CHANNELNOTFOUND(nick, channel) 			(": 403 " + nick + " " + channel + " :No such channel" + CRLF)
#define ERR_NOTOPERATOR(channel) 					(": 482 #" + channel + " :You're not a channel operator" + CRLF)
#define ERR_NOSUCHNICK(channel, name) 				(": 401 #" + channel + " " + name + " :No such nick/channel" + CRLF )
#define ERR_INCORPASS(nick) 						(": 464 " + nick + " :Password incorrect !" + CRLF )
#define ERR_ALREADYREGISTERED(nick) 				(": 462 " + nick + " :You may not reregister !" + CRLF )
#define ERR_NOnick(nick) 							(": 431 " + nick + " :No nick given" + CRLF )
#define ERR_clientNickINUSE(nick) 					(": 433 " + nick + " :nick is already in use" + CRLF)
#define ERR_ERRONEUSNICK(nick) 						(": 432 " + nick + " :Erroneus nick" + CRLF)
#define ERR_NOTREGISTERED(nick) 					(": 451 " + nick + " :You have not registered!" + CRLF)
#define ERR_CMDNOTFOUND(nick, command) 				(": 421 " + nick + " " + command + " :Unknown command" + CRLF)
#endif