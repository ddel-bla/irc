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
#define WHO     "WHO"
#define PART    "PART"
#define TRIVIAL "!TRIVIAL"

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
#define INVEX       "I"                 // Server supports invite exceptions

/* CHARACTERS*/
#define CRLF "\r\n"

/* TIMEOUT */
#define REGISTRATION_TIMEOUT 45

/* QUIT default reason*/
#define QUIT_DEFAULT "QUIT :Leaving"

/* ----- SYSTEM MESSAGES ----- */
/* welcome */
#define RPL_CONNECTED(nick)							(": 001 " + nick + " : Welcome to the IRC server!" + CRLF)
#define RPL_YOURHOST(nick)                          (": 002 " + nick + " : Your host is " + SERVERNAME + ", running version " + VERSION + CRLF)
#define RPL_CREATE(nick, date)                      (": 003 " + nick + " : This server was created " + date + CRLF)
#define RPL_MYINFO(nick)                            (": 004 " + nick + " : " +  SERVERNAME + " " + VERSION + CRLF)
#define RPL_ISUPPORT(nick)                          (": 005 " + nick + " : 11 tokens are supported by this server" + CRLF)

/* channel */
#define RPL_CHANNELMODEIS(nick, channel, modes)     (": 324 " + nick + " #" + channel + " " + modes + CRLF)
#define RPL_CREATIONTIME(nick, channel, time) 		(": 329 " + nick + " #" + channel + " " + time + CRLF)
#define RPL_NOTOPIC(nick, channel)                  (": 331 " + nick + " #" + channel + " :No topic is set" + CRLF)
#define RPL_TOPIC(nick, channel, topic)             (": 332 " + nick + " #" + channel + " :" + topic + CRLF)
#define RPL_INVITING(nick, invited, channel)        (": 341 " + nick + " " + invited + " #" + channel + CRLF)
#define RPL_NAMREPLY(nick, channel, clientslist) 	(": 353 " + nick + " @ #" + channel + " :" + clientslist + CRLF)
#define RPL_ENDOFNAMES(nick, channel) 				(": 366 " + nick + " #" + channel + " :END of /NAMES list" + CRLF)

/* own */
#define RPL_JOINMSG(host, ipaddress, channel) 		(":" + host + "@" + ipaddress + " JOIN #" + channel + CRLF)
#define RPL_CHANGEDNICK(nick)						(": 990 " + nick + " : Nickname changed!" + CRLF)

/* SYSTEM ERRORS */
#define ERR_NOSUCHNICK(name) 				        (": 401 " + name + " :No such nick/channel" + CRLF )
#define ERR_NOSUCHCHANNEL(nick, channel) 			(": 403 " + nick + " #" + channel + " :No such channel" + CRLF)
#define ERR_TOOMANYCHANNELS(nick)                   (": 405 " + nick + " :You have joined too many channels!" + CRLF)
#define ERR_TOOMANYTARGETS(nick)                    (": 407 " + nick + " :Too many targets!" + CRLF)
#define ERR_NORECIPIENT(nick, command)              (": 411 " + nick + " :No recipient given " + command + CRLF)
#define ERR_NOTEXTTOSEND(nick)                      (": 412 " + nick + " :No text to send" + CRLF)
#define ERR_UNKNOWNCOMMAND(nick, command)           (": 421 " + nick + " " + command + " :Unknown command" + CRLF)
#define ERR_NONICKNAME(nick)                        (": 431 " + nick + " :No nickname given" + CRLF )
#define ERR_NICKNAMEINUSE(nick) 					(": 433 " + nick + " :nick is already in use" + CRLF)
#define ERR_ERRONEUSNICK(nick) 						(": 432 " + nick + " :Erroneus nick" + CRLF)
#define ERR_USERNOTINCHANNEL(nick, kicked, channel) (": 441 " + nick + " " + kicked + " #" + channel + " :They aren't on that channel" + CRLF)
#define ERR_NOTONCHANNEL(nick, channel)             (": 442 " + nick + " #" + channel + " :You're not on that channel" + CRLF)
#define ERR_USERONCHANNEL(nick, invited, channel)   (": 443 " + nick + " " + invited + " #" + channel + " :is already on channel" + CRLF)
#define ERR_NOTREGISTERED(nick) 					(": 451 " + nick + " :You have not registered!" + CRLF)
#define ERR_NEEDMOREPARAMS(nick)                    (": 461 " + nick + " :Not enough parameters." + CRLF)
#define ERR_ALREADYREGISTERED(nick) 				(": 462 " + nick + " :You may not reregister !" + CRLF)
#define ERR_PASSWDMISMATCH(nick) 				    (": 464 " + nick + " :Password incorrect !" + CRLF)
#define ERR_CHANNELISFULL(nick, channel)            (": 471 " + nick + " #" + channel + " :Cannot join channel (+l)" + CRLF)
#define ERR_UNKNOWNMODE(nick, channel, mode)	 	(": 472 " + nick + " #" + channel + " " + mode + " :is not a recognised channel mode" + CRLF)
#define ERR_INVITEONLYCHAN(nick, channel)           (": 473 " + nick + " #" + channel + " :Cannot join channel (+i)" + CRLF)
#define ERR_BADCHANNELKEY(nick, channel)            (": 475 " + nick + " #" + channel + " :Cannot join channel (+k)"+ CRLF)
#define ERR_CHANOPRIVSNEEDED(nick, channel)         (": 482 " + nick + " #" + channel + " :You're not a channel operator" + CRLF)
#define ERR_INVALIDKEY(nick, channel)               (": 525 " + nick + " #" + channel + " :Key is not well-formed" + CRLF)
#define ERR_INVALIDLIMIT(nick, channel)             (": 525 " + nick + " #" + channel + " :Limit is not valid" + CRLF)
#define ERR_NOTANOPERATOR(nick, channel)            (": 525 " + nick + " #" + channel + " :Can't remove not an operator" + CRLF)
#define ERR_INVALIDMODEPARAM(nick, channel, cmd)    (": 696 " + nick + " #" + channel + " " + cmd + " :Not a valid mode" + CRLF)
#define ERR_USERTOOLONG(nick)                       (": 991 " + nick + " :Username too long! ( max length: " + Utils::intToString(USERLEN) + ")" + CRLF)
#define ERR_NICKTOOLONG(nick)                       (": 992 " + nick + " :Nick too long! ( max length: " + Utils::intToString(NICKLEN) + " )" + CRLF)
#define ERR_INVALIDCHANNAME(channel)                (": 993 #" + channel + " :Invalid channel name! ('#', '&' not allowed, max length: " + Utils::intToString(CHANNELLEN) + " )" + CRLF)
#define ERR_TOOMANYPARAMS(nick, command)            (": 989 " + nick + " '" + command + "' :Too many parameters" + CRLF)

/* ----- TRIVIAL BOT ----- */
/* QUESTIONS FILE */
#define F_QUESTIONS "resources/questions.txt"
#define NUM_QUESTIONS 5

/* MSG */
#define BOT_WELCOME(nick)                   (": 000 " + nick + " :Welcome to the trivial game! Good luck!" + CRLF)
#define BOT_CORRECTANSWER(nick, score)      (": 000 " + nick + " :That's right! Your current score is: " + Utils::intToString(score) + "!" + CRLF)
#define BOT_INCORRECTANSWER(nick, answer)   (": 000 " + nick + " :Incorrect! The correct answer was: " + answer + " :( " + CRLF)
#define BOT_ENDGAME(nick, score)            (": 000 " + nick + " :Game over! Your final score was: " + Utils::intToString(score) + " out of " + Utils::intToString(NUM_QUESTIONS) + " XD" + CRLF)
#define BOT_QUITGAME(nick)                  (": 000 " + nick + " :Bye, thanks for playing!" + CRLF)
#define BOT_ERR_NOTPLAYING(nick)            (": 000 " + nick + " :You are not playing right now! Use !trivial start." + CRLF)
#define BOT_ERR_ALREDYPLAYING(nick)         (": 000 " + nick + " :You are already playing!" + CRLF)
#define BOT_ERR_QUESTIONSNOTFOUND(nick)     (": 000 " + nick + " :No questions found! Ending game..." + CRLF)
#define BOT_ERR_NOANSWERGIVEN(nick)         (": 000 " + nick + " :Please specify your answer. Example: !trivial answer <number>." + CRLF)

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
