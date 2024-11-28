#ifndef MACROS_HPP
# define MACROS_HPP

/* COMMANDS */
#define PASS    "PASS"
#define NICK    "NICK"
#define USER    "USER"
#define QUIT    "QUIT"
#define JOIN    "JOIN"
#define PRIVMSG "PRIVMSG"
#define KICK    "KICK"
#define INVITE  "INVITE"
#define TOPIC   "TOPIC"
#define MODE    "MODE"

/* CHARACTERS*/
#define CRLF "\r\n"

/* SYSTEM MESSAGES */
#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to the IRC server!" + CRLF)
#define RPL_UMODEIS(hostname, channelname, mode, user)  ":" + hostname + " MODE " + channelname + " " + mode + " " + user + CRLF
#define RPL_CREATIONTIME(nickname, channelname, creationtime) ": 329 " + nickname + " #" + channelname + " " + creationtime + CRLF
#define RPL_CHANNELMODES(nickname, channelname, modes) ": 324 " + nickname + " #" + channelname + " " + modes + CRLF
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE #" + channelname + " " + mode + " " + arguments + CRLF)
#define RPL_clientNickCHANGE(oldnickname, nickname) (":" + oldnickname + " NICK " + nickname + CRLF)
#define RPL_JOINMSG(hostname, ipaddress, channelname) (":" + hostname + "@" + ipaddress + " JOIN #" + channelname + CRLF)
#define RPL_NAMREPLY(nickname, channelname, clientslist) (": 353 " + nickname + " @ #" + channelname + " :" + clientslist + CRLF)
#define RPL_ENDOFNAMES(nickname, channelname) (": 366 " + nickname + " #" + channelname + " :END of /NAMES list" + CRLF)
#define RPL_TOPICIS(nickname, channelname, topic) (": 332 " + nickname + " #" +channelname + " :" + topic + CRLF)

/* SYSTEM ERRORS */
#define ERR_NEEDMODEPARM(channelname, mode) (": 696 #" + channelname + " * You must specify a parameter for the key mode. " + mode + CRLF)
#define ERR_INVALIDMODEPARM(channelname, mode) ": 696 #" + channelname + " Invalid mode parameter. " + mode + CRLF
#define ERR_KEYSET(channelname) ": 467 #" + channelname + " Channel key already set. " + CRLF
#define ERR_UNKNOWNMODE(nickname, channelname, mode) ": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + CRLF
#define ERR_NEEDMOREPARAMS(nickname) (": 461 " + nickname + " :Not enough parameters." + CRLF)
#define ERR_CHANNELNOTFOUND(nickname, channelname) (": 403 " + nickname + " " + channelname + " :No such channel" + CRLF)
#define ERR_NOTOPERATOR(channelname) (": 482 #" + channelname + " :You're not a channel operator" + CRLF)
#define ERR_NOSUCHNICK(channelname, name) (": 401 #" + channelname + " " + name + " :No such nick/channel" + CRLF )
#define ERR_PASSWDMISMATCH(nickname) (": 464 " + nickname + " :Password incorrect !" + CRLF )
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + CRLF )
#define ERR_NONICKNAME(nickname) (": 431 " + nickname + " :No nickname given" + CRLF )
#define ERR_clientNickINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneus nickname" + CRLF)
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + CRLF)
#define ERR_UNKNOWNCOMMAND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command" + CRLF)
#define ERR_NICKCOLLISION(nickname) (": 426 " + nickname + "Nickname collision KILL from <user>@<host>") // TODO

/* COLORS */
#define RED		"\033[0;91m"
#define GREEN   "\033[1;92m"
#define YELLOW	"\033[1;93m"
#define BLUE	"\033[0;94m"
#define MAGENTA	"\033[0;95m"
#define CYAN	"\033[0;96m"
#define WHITE	"\033[0;97m"
#define RESET	"\033[0m"

#endif  // MACROS.HPP