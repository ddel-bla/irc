#include "IRCServer.hpp"

/* -- REGISTRATION PROCESS -- */

void	IRCServer::checkRegistrationTimeout(void)
{
    std::time_t now = std::time(NULL);
    
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ) {
        Client* client = it->second;

        if (!client->isRegistred() && !client->isDisconnected() && std::difftime(now, client->getConnectionTime()) > REGISTRATION_TIMEOUT) {
			logger.info("[TIMEOUT] " + client->fdToString() + " did not complete the registered on time. Disconecting...");
			close(it->first);
			removeFds(it->first);
            delete client;
            
            std::map<int, Client*>::iterator toErase = it;
            ++it;
			clients.erase(toErase);
        } else {
            ++it; // Avanzar al siguiente iterador si no se borra
        }
    }
}

/* PASS CMD */
void	IRCServer::authenticate(std::string command, Client& client)
{
	logger.info("[PASS] " + client.fdToString() + " --> Authenticating...");
	
	// 1. Split command
	std::vector<std::string> split_command= Utils::splitBySpaces(command);;
    std::string enteredPassword;

    if (split_command.size() == 2 && !client.isRegistred())
    {
        // 2. Skip 'PASS' & REMOVE ':' -- PASS :password
		enteredPassword = Utils::removeLeadingChar(split_command[1], ':');
		
		// 3. Valid passwd
        if (enteredPassword == this->password)
		{
			logger.info("[PASS] " + client.fdToString() + " --> Valid password.");
			client.setAutenticate(true);
		}
        // 3. Wrong passwd
		else
        {
			logger.warning("[PASS] " + client.fdToString() + " --> Wrong password.");
            message.sendToClient(client.getFd(), ERR_PASSWDMISMATCH(client.getNickname()));
            client.setAutenticate(false);
        }
    }
    else if (client.isRegistred())	// ALREADY REGISTERED
	{
		logger.warning("[PASS] " + client.fdToString() + " --> Already registered!");
		message.sendToClient(client.getFd(), ERR_ALREADYREGISTERED(client.getNickname()));
	}	
	else							// NEED MORE PARAMS
	{
		logger.warning("[PASS] " + client.fdToString() + " --> Need more params!");
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
	}
}

/* NICK CMD */
void	IRCServer::registerNickname(std::string command, Client& client)
{
	logger.info("[NICK] " + client.fdToString() + " --> Setting nickname...");
	//1. Authenticated (PASS)
	if (client.isAutenticate())
	{
		std::vector<std::string> split_command = Utils::splitBySpaces(command);
		size_t command_len = split_command.size(); 
		if (command_len == 2)
		{
			//2. Skip 'NICK' & remove ':' NICK :wii
			std::string	nickname = Utils::removeLeadingChar(split_command[1], ':');

			if (!isValidNickname(nickname))	// Valid characters
			{
				logger.warning("[NICK] " + client.fdToString() + " --> '" + nickname +"' is invalid.");
				message.sendToClient(client.getFd(), ERR_ERRONEUSNICK(nickname));
			}
			else if (nickname.size() > NICKLEN)	// Valid length
			{
				logger.warning("[NICK] " + client.fdToString() + " --> '" + nickname +"' too long.");
				message.sendToClient(client.getFd(), ERR_NICKTOOLONG(nickname));
			}
			else if (isNicknameTaken(nickname))	// Not taken
			{
				logger.warning("[NICK] " + client.fdToString() + " --> '" + nickname +"' in use.");
				message.sendToClient(client.getFd(), ERR_NICKNAMEINUSE(nickname));
			}
			else
			{	
				std::string	oldNickname;
				
				// 3. Has already a nick --> changing nickname
				if (!client.getNickname().empty() && nickname != client.getNickname())
					oldNickname = client.getNickname();
				
				logger.info("[NICK] " + client.fdToString() + " --> Registered with nickname '" + nickname + "'.");
				client.setNickname(nickname);
				
				// 4. Channel clients update
				logger.info("[NICK] " + client.fdToString() + " --> Changing nickname in channels...");
				updateChannelsClientNickname(client.getFd(), nickname);

				// 5. Set registered and succecollisionss msg
				if (!client.getUsername().empty())
				{
					if (!client.isRegistred()) // If already registered dont send
						sendwelcomeMessage(client.getFd(), client.getNickname());
					else
						logger.info("[NICK] User " + nickname + " registered successfully.");
					client.setRegistred(true);
					if (!oldNickname.empty())
					{	
						logger.info("[NICK] "  + (oldNickname) + " changed his nickname to '" + nickname + "'.");
						message.sendToClient(client.getFd(), RPL_CHANGEDNICK(oldNickname));
					}
				}
			}
		}
		else if (command_len < 2) 	// Not enough params
		{
			logger.warning("[NICK] " + client.fdToString() + " --> Need more params!");
			message.sendToClient(client.getFd(), ERR_NONICKNAME(std::string("*")));

		}
		else if (command_len > 2)	// Spaces not allowed
		{
			logger.warning("[NICK] " + client.fdToString() + " --> Too many params!");
			message.sendToClient(client.getFd(), ERR_ERRONEUSNICK(std::string("*")));
		}
	}
	else
	{
		logger.warning("[NICK] " + client.fdToString() + " --> Not authenticated!");
		message.sendToClient(client.getFd(), ERR_NOTREGISTERED(std::string("*")));
	}
}

/* USER CMD */
void	IRCServer::registerUsername(std::string command, Client& client)
{
	logger.info("[USER] " + client.fdToString() + " --> Setting username...");
	// 1. Autenticated
	if(client.isAutenticate())
	{
		// 2. Split command
		std::vector<std::string> split_command = Utils::splitBySpaces(command);
		size_t command_len = split_command.size();
		if (command_len < 5)			// NEED MORE PARAMS
		{
			logger.warning("[USER] " + client.fdToString() + " --> Need more params!");
			message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
		}
		else if (!client.getUsername().empty())	// ALREADY REGISTERED
		{
			logger.warning("[USER] " + client.fdToString() + " --> Too many params!");
			message.sendToClient(client.getFd(), ERR_ALREADYREGISTERED(client.getNickname()));
		}
		else
		{
			// 3. Set username
			if (split_command[1].size() < USERLEN)
			{
				logger.info("[USER] " + client.fdToString() + " --> Registered with username '" + split_command[1] + "'.");
				client.setUsername(split_command[1]);
				registerRealname(command, client);
				
				// 4. Set registered to true if NICK and USER completed
				if (!client.getNickname().empty())
				{
					client.setRegistred(true);
					logger.info("[USER] User " + client.getUsername() + " registered successfully.");
					sendwelcomeMessage(client.getFd(), client.getNickname());
				}
			}
			else
			{
				logger.warning("[USER] " + client.fdToString() + " --> '" + split_command[1] + "' Name too long!");
				message.sendToClient(client.getFd(), ERR_USERTOOLONG(split_command[1]));
			}
		}
	}
	else		// NO AUTENTICATED
	{
		logger.warning("[USER] " + client.fdToString() + " --> Not authenticated!");
		message.sendToClient(client.getFd(), ERR_NOTREGISTERED(std::string("*")));
	}
}

/* SYSTEM MESSAGES */
void IRCServer::sendwelcomeMessage(int fd, const std::string& nickname)
{
	message.sendToClient(fd, RPL_CONNECTED(nickname));
	message.sendToClient(fd, RPL_YOURHOST(nickname));
	message.sendToClient(fd, RPL_CREATE(nickname, this->creationDate));
	message.sendToClient(fd, RPL_MYINFO(nickname));
	sendRplISupport(fd, nickname);
}

void	IRCServer::sendRplISupport(int fd, const std::string& nickname)
{
	message.sendToClient(fd, RPL_ISUPPORT(nickname));

    std::string msg = "  RPL_ISUPPORT Parameters for " + nickname + ":\n";
    msg += "  CHANLIMIT=" + Utils::intToString(CHANLIMIT) + " (Max channels a client may join)\n";
    msg += "  CHANMODES=" + std::string(CHANMODES) + " (Available channel modes)\n";
    msg += "  CHANNELLEN=" + Utils::intToString(CHANNELLEN) + " (Max length of a channel name)\n";
    msg += "  CHANTYPES=" + std::string(CHANTYPES) + " (Channel prefixes available)\n";
    msg += "  KICKLEN=" + Utils::intToString(KICKLEN) + " (Max length of the <reason> for KICK cmd)\n";
    msg += "  MAXTARGETS=" + Utils::intToString(MAXTARGETS) + " (Max number of targets for PRIVMSG)\n";
    msg += "  NETWORK=" + std::string(NETWORK) + " (Name of the IRC network)\n";
    msg += "  NICKLEN=" + Utils::intToString(NICKLEN) + " (Max length of a nickname)\n";
    msg += "  TARGMAX=" + std::string(TARGMAX) + " (Max number of targets allowed for commands)\n";
    msg += "  TOPICLEN=" + Utils::intToString(TOPICLEN) + " (Max length of a topic)\n";
    msg += "  USERLEN=" + Utils::intToString(USERLEN) + " (Max length of a username)\n";
	msg += "  INVEX=" + std::string(INVEX) + " (Server supports invite exceptions)\n" + CRLF;
    message.sendToClient(fd, msg);
}

bool	IRCServer::isValidNickname(const std::string nickname)
{	
	// VALID CHARACTERS
    if (nickname.empty() || nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
        return (false);

    for (size_t i = 0; i < nickname.size(); i++) {
        if (!std::isalnum(nickname[i]) && nickname[i] != ' ')
            return (false);
    }

    return (true);
}

bool	IRCServer::isNicknameTaken(const std::string nickname)
{
	std::map<int, Client*>:: const_iterator it;
	Client* cliente;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
        cliente = it->second;
        if (cliente && cliente->getNickname() == nickname) {
            return (true);
        }
    }

	return (false);
}

void	IRCServer::registerRealname(std::string& command, Client& client)
{
	std::istringstream  ss(command);
    std::string         word;
    std::string         realname;

    // SKIP FIRST FOUR WORDS
    ss >> word;
    ss >> word;
	ss >> word;
	ss >> word;

	std::getline(ss, realname); 

    // DELETE LEADING ':'
    if (!realname.empty() && realname[0] == ' ')
        realname = realname.substr(1);

    client.setRealname(realname);
}

void IRCServer::updateChannelsClientNickname(int fd, const std::string& newNickname)
{
	for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel& channel = it->second;

        std::map<int, Client*>::const_iterator memberIt = channel.getMembers().find(fd);
        if (memberIt != channel.getMembers().end()) {
            memberIt->second->setNickname(newNickname);
        }

		std::map<int, Client*>::const_iterator operatorIt = channel.getOperators().find(fd);
        if (operatorIt != channel.getOperators().end()) {
            operatorIt->second->setNickname(newNickname);
        }
    }
}

