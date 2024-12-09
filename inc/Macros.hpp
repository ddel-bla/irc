#ifndef MACROS_HPP
# define MACROS_HPP

/* VERSION */
#define VERSION     "1.0.0"
#define SERVERNAME  "Servidor Sirviente"

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

/* RPL_ISUPPORT */
#define CHANLIMIT   5                   // Number of channels a client may join
#define CHANMODES   "i,t,k,o,l"         // Channles modes available
#define CHANNELLEN  20                  // Maximum length of a channel name
#define CHANTYPES   "#"                 // Channel prefixes available
#define KICKLEN     50                  // Maximun length for the <reason> of KICK cmd
#define MAXTARGETS  4                   // Maximun number of targets a PRIVMSG
#define NETWORK     "42 net -.-"        // Name of the IRC network
#define NICKLEN     9                   // Maximun length of a nickname
#define TARGMAX     "PRIVMSG:4,JOIN:4"  // Maximun number of targets allowed
#define TOPICLEN    100                 // Maximun length of a topic
#define USERLEN     12                  // Maximun length of a username

/* CHARACTERS*/
#define CRLF "\r\n"

/* TIMEOUT */
#define REGISTRATION_TIMEOUT 45

/* QUIT default reason*/
#define QUIT_DEFAULT "QUIT :Leaving"

/* SYSTEM MESSAGES */
#define RPL_CONNECTED(nick)							(": 001 " + nick + " : Welcome to the IRC server!" + CRLF)
#define RPL_YOURHOST(nick)                          (": 002 " + nick + " : Your host is " + SERVERNAME + ", running version " + VERSION + CRLF)
#define RPL_CREATE(nick, date)                      (": 003 " + nick + " : This server was created " + date + CRLF)
#define RPL_MYINFO(nick)                            (": 004 " + nick + " : " +  SERVERNAME + " " + VERSION + CRLF)
#define RPL_ISUPPORT(nick)                          (": 005 " + nick + " : 11 tokens :are supported by this server" + CRLF)
#define RPL_UMODEIS(host, channel, mode, user)  	":" + host + " MODE " + channel + " " + mode + " " + user + CRLF
#define RPL_CREATIONTIME(nick, channel, time) 		": 329 " + nick + " #" + channel + " " + time + CRLF
#define RPL_CHANNELMODES(nick, channel, modes) 		": 324 " + nick + " #" + channel + " " + modes + CRLF
#define RPL_CHANGEMODE(host, channel, mode, args) 	(":" + host + " MODE #" + channel + " " + mode + " " + args + CRLF)
#define RPL_clientNickCHANGE(oldnick, nick) 		(":" + oldnick + " NICK " + nick + CRLF)
#define RPL_JOINMSG(host, ipaddress, channel) 		(":" + host + "@" + ipaddress + " JOIN #" + channel + CRLF)
#define RPL_NAMREPLY(nick, channel, clientslist) 	(": 353 " + nick + " @ #" + channel + " :" + clientslist + CRLF)
#define RPL_ENDOFNAMES(nick, channel) 				(": 366 " + nick + " #" + channel + " :END of /NAMES list" + CRLF)
#define RPL_TOPICIS(nick, channel, topic) 			(": 332 " + nick + " #" +channel + " :" + topic + CRLF)
#define RPL_CHANGEDNICK(nick)						(": 990 " + nick + " : Nickname changed!" + CRLF)

/* SYSTEM ERRORS */
#define ERR_NEEDMODEPARM(channel, mode) 			(": 696 #" + channel + " * You must specify a parameter for the key mode. " + mode + CRLF)
#define ERR_INVALIDMODEPARM(channel, mode) 			": 696 #" + channel + " Invalid mode parameter. " + mode + CRLF
#define ERR_KEYSET(channel) 						": 467 #" + channel + " Channel key already set. " + CRLF
#define ERR_UNKNOWNMODE(nick, channel, mode)	 	": 472 " + nick + " #" + channel + " " + mode + " :is not a recognised channel mode" + CRLF
#define ERR_NEEDMOREPARAMS(nick)                    (": 461 " + nick + " :Not enough parameters." + CRLF)
#define ERR_PASSWDMISMATCH(nick)                    (": 464 " + nick + " :Password incorrect !" + CRLF )
#define ERR_NOSUCHCHANNEL(nick, channel) 			(": 403 " + nick + " #" + channel + " :No such channel" + CRLF)
#define ERR_CHANOPRIVSNEEDED(nick, channel)         (": 482 " + nick + " #" + channel + " :You're not a channel operator" + CRLF)
#define ERR_NOSUCHNICK(name) 				        (": 401 " + name + " :No such nick/channel" + CRLF )
#define ERR_INCORPASS(nick) 						(": 464 " + nick + " :Password incorrect !" + CRLF )
#define ERR_ALREADYREGISTERED(nick) 				(": 462 " + nick + " :You may not reregister !" + CRLF )
#define ERR_NONICKNAME(nick)                        (": 431 " + nick + " :No nickname given" + CRLF )
#define ERR_clientNickINUSE(nick) 					(": 433 " + nick + " :nick is already in use" + CRLF)
#define ERR_ERRONEUSNICK(nick) 						(": 432 " + nick + " :Erroneus nick" + CRLF)
#define ERR_NOTREGISTERED(nick) 					(": 451 " + nick + " :You have not registered!" + CRLF)
#define ERR_UNKNOWNCOMMAND(nick, command)           (": 421 " + nick + " " + command + " :Unknown command" + CRLF)
#define ERR_NICKCOLLISION(nick)                     (": 426 " + nick + " Nickname collision KILL from  user + @ + host" + CRLF) // TODO
#define ERR_NORECIPIENT(nick, command)              (": 411 " + nick + " :No recipient given " + command + CRLF)
#define ERR_NOTEXTTOSEND(nick)                      (": 412 " + nick + " :No text to send" + CRLF)
#define ERR_BADCHANNELKEY(nick, channel)            (": 475 " + nick + " " + channel + " Cannot join channel (+k)"+ CRLF) // TODO
#define ERR_TOOMANYTARGETS(nick)                    (": 407 " + nick + " :Too many targets!" + CRLF)
#define ERR_TOOMANYCHANNELS(nick)                   (": 405 " + nick + " :You have joined too many channels!" + CRLF)
#define ERR_USERTOOLONG(nick)                       (": 991 " + nick + " :Username too long! ( max length: " + Utils::intToString(USERLEN) + ")" + CRLF)
#define ERR_NICKTOOLONG(nick)                       (": 992 " + nick + " :Nick too long! ( max length: " + Utils::intToString(NICKLEN) + " )" + CRLF)
#define ERR_INVALIDCHANNAME(channel)                (": 993 #" + channel + " :Invalid channel name! ('#', '&' not allowed, max length: " + Utils::intToString(CHANNELLEN) + " )" + CRLF)
#define ERR_NOTONCHANNEL(nick, channel)             (": 442 " + nick + " #" + channel + " :You're not on that channel" + CRLF)
#define ERR_USERNOTINCHANNEL(nick, kicked, channel) (": 441 " + nick + " " + kicked + " #" + channel + " :They aren't on that channel" + CRLF)

/* COLORS */
#define RED		"\033[0;91m"
#define GREEN   "\033[1;92m"
#define YELLOW	"\033[1;93m"
#define BLUE	"\033[0;94m"
#define MAGENTA	"\033[0;95m"
#define CYAN	"\033[0;96m"
#define WHITE	"\033[0;97m"
#define BOLD    "\033[1m"
#define RESET	"\033[0m"


#endif  // MACROS.HPP
