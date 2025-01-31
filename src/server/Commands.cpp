#include "IRCServer.hpp"

/* MAIN METHOD */
void IRCServer::process_command(std::string command, Client& client)
{
	std::vector<std::string> split_command = Utils::splitBySpaces(command);

	if (command.empty())
		return ;

	// EMPTY VECTOR
	if (!split_command.size())
		return ;
	
	// Handle commands using a switch
	Command cmd = commandToInt(Utils::toUpper(split_command[0]));
	
	switch (cmd)
	{
		case PASS:
			authenticate(command, client);
			break;
		
		case USER:
			registerUsername(command, client);
			break;
		
		case NICK:
			registerNickname(command, client);
			break;
		
		default:
			if (!client.isRegistred())
			{
				logger.warning("User is not registered");
				message.sendToClient(client.getFd(), ERR_NOTREGISTERED(client.getNickname()));
			}
			break;
	}

	if (client.isRegistred()) // Si está registrado puede ejecutar otros comandos
	{
		switch (cmd)
		{
			case PRIVMSG:
				privMsg(command, client);
				break;
			case JOIN:
				join(command, client);
				break;
			case KICK:
				kick(command, client);
				break;
			case INVITE:
				invite(command, client);
				break;
			case TOPIC:
				topic(command, client);
				break;
			case MODE:
				mode(command, client);
				break;
			case WHO:
				who(command, client);
				break;
			case PART:
				part(command, client);
				break;
			case UNKNOWN:
				logger.warning("Command not found");
				message.sendToClient(client.getFd(), ERR_UNKNOWNCOMMAND(client.getNickname(), command));
				break;
			default:
				break;
		}
	}
}

/* PRIVMSG */
void IRCServer::privMsg(const std::string& command, Client& client)
{
	logger.info("[PRIVMSG] :: " + client.getNickname());

	// 1. Splitting command
	std::vector<std::string> split_command = Utils::splitBySpaces(command);
	int	command_len = split_command.size();

	if (command_len >= 3)
	{
		// 2. Extracting destinataries
		std::vector<std::string> destinataries = Utils::split(split_command[1], ",");
		if (destinataries.size() > MAXTARGETS)	// Limit destinataries
		{
			logger.warning("[PRIVMSG] " + client.getNickname() + " selected too many targets (" + Utils::intToString(destinataries.size()) + ")");
			message.sendToClient(client.getFd(), ERR_TOOMANYTARGETS(client.getNickname()));
			return;
		}

		// 3. Format msg (hexchat)
		std::string	msg_text = hx_generic_format(command, client);
		logger.info("[PRIVMSG] :: Sending message: " + msg_text);

		// 4. Send msg to each destinatary
		for (std::vector<std::string>::iterator it = destinataries.begin(); it != destinataries.end(); ++it)
		{
			const std::string& dest_nick = *it;
			int dest_fd = -1;
			
			// 5. Empty destinatary or #
			if (dest_nick.empty() || (dest_nick.size() == 1 && dest_nick[0] == '#'))
			{
				logger.info("[PRIVMSG] Empty dest.");
				continue;
			}
			// 5. Destinatary is a channel
			else if (dest_nick.size() > 1 && dest_nick[0] == '#')
			{
				std::string realName = Utils::removeLeadingChar(dest_nick, '#');
				std::map<std::string, Channel>::iterator channel = channels.find(realName);
				if (!existsChannelByName(realName))		// does not exist
				{
					logger.warning("[PRIVMSG] :: Channel '" + realName + " not found.");
					message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), dest_nick));
					continue;
				}
				else if (!channel->second.isMember(client.getFd()))
				{
					logger.warning("[PRIVMSG] :: Client : " + client.getNickname() + " is not in channel '" + realName + "'.");
            		message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), realName));
            		continue ;
				}
				// 6. Send msg to channel
				message.sendToChannel(realName, msg_text, channels, client.getFd());
			}
			// 5. Destinatary is a user
			else
			{
				dest_fd = findFdByNickname(dest_nick);
				if (dest_fd == -1)		// does not exist
				{	
					logger.warning("[PRIVMSG] User '" + dest_nick + " not found.");
					message.sendToClient(client.getFd(), ERR_NOSUCHNICK(dest_nick));
					continue;
				}
				// 6. Send msg to client
				message.sendToClient(dest_fd, msg_text);
			}
    	}
	}
	else if (command_len == 2) 	// NO MSG
	{
		logger.warning("[PRIVMSG] " + client.getNickname() + " --> No message in command.");
		message.sendToClient(client.getFd(), ERR_NOTEXTTOSEND(client.getNickname()));
	}
	else						// NO DESTINATARY
	{
		logger.warning("[PRIVMSG] " + client.getNickname() + " --> No destinatary.");
		message.sendToClient(client.getFd(), ERR_NORECIPIENT(client.getNickname(), command));
	}
}

int IRCServer::findFdByNickname(const std::string& nickname) {
	
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return it->first;
		}
	}

	return -1;
}

bool IRCServer::existsChannelByName(const std::string& name)
{	
	std::map<std::string, Channel>::iterator it = channels.find(name);
	if (it == channels.end()) {
		return false;
	}
	return true;
}

/* JOIN CHANNEL */
void	IRCServer::join(const std::string& command, Client& client)
{
	logger.info("[JOIN]:: User " + client.getNickname() + " joining a channel.");

	// 1. Splitting cmd
	std::vector<std::string> split_command = Utils::splitBySpaces(command);
	int	command_len = split_command.size();

	if (command_len >= 2)
	{	
		// 2. Extracting channels & passwords
		std::vector<std::string> cmd_channels = Utils::split(split_command[1], ",");
		if (cmd_channels.size() > MAXTARGETS)
		{
			logger.warning("[JOIN] " + client.getNickname() + " selected too many targets (" + Utils::intToString(cmd_channels.size()) + ")");
			message.sendToClient(client.getFd(), ERR_TOOMANYTARGETS(client.getNickname()));
			return;
		}
		
		std::vector<std::string> cmd_keys;
		if (command_len == 3)
			cmd_keys = Utils::split(split_command[2], ",");
		
		// 4. Join each channel (if possible) 
		for (size_t i = 0; i < cmd_channels.size(); ++i)
		{	
			// 5. Get channel name and key
			std::string channelName = Utils::removeLeadingChar(cmd_channels[i], '#');
			if (channelName.empty() || (channelName.size() == 1 && channelName[0] == '#'))
			{
				logger.info("[JOIN] Empty channel.");
				continue;
			}
    		std::string channelKey = (i < cmd_keys.size()) ? cmd_keys[i] : "";

			// 3. Format msg (hexchat)
			std::string msg_text = hx_join_format(channelName, client, false);
			logger.info("[JOIN] :: HC Message: " + msg_text);

    		// 6. Search in existing channels
    		std::map<std::string, Channel>::iterator ch = channels.find(channelName);
			
			// 7. Channel does not exist
			if (ch == channels.end())
			{
				if (!isChannelNameValid(channelName)) // Valid channel name
				{
					logger.warning("[JOIN] :: Wrong channel name : " + channelName + ".");
					message.sendToClient(client.getFd(), ERR_INVALIDCHANNAME(channelName));
					continue; // Skip to the next channel
				}
				else if (client.getChannelCount() + 1 > CHANLIMIT)	// Channel limit
				{
					logger.warning("[JOIN] :: User: " + client.getNickname() + " has reached the maximum number of channels.");
					message.sendToClient(client.getFd(), ERR_TOOMANYCHANNELS(client.getNickname()));
					continue; // Skip to the next channel
				}
				
				logger.info("[JOIN] :: Creating channel : " + channelName + " " + channelKey);
				if (!channelKey.empty())	// PASSWD
					channels.insert(std::make_pair(channelName, Channel(channelName, channelKey)));
				else						// NO PASSWD
					channels.insert(std::make_pair(channelName, Channel(channelName)));

				// 8. User becomes operator
				ch = channels.find(channelName);
				ch->second.addOperator(client.getFd(), &client);				
    		} 
			// 7. Channel does exist
			else
			{
				// 8. Wrong key entered
				if (!ch->second.getChannelKey().empty() && (channelKey.empty() || ch->second.getChannelKey() != channelKey)) {
					logger.info("[JOIN] :: Wrong channel key : " + channelName + ". Entered key: " + channelKey + ", Actual key: " + ch->second.getChannelKey());
					message.sendToClient(client.getFd(), ERR_BADCHANNELKEY(client.getNickname(), ch->second.getName()));
					continue; // Skip to the next channel
				}

				// 9. Invite-only channel
				if (ch->second.isInviteOnly() && !(ch->second.isInvited(client.getFd()) || ch->second.isOperator(client.getFd())))
				{
					logger.info("[JOIN] :: Client : " + client.getNickname() + " hasn't been invited to: " + channelName + ".");
					message.sendToClient(client.getFd(), ERR_INVITEONLYCHAN(client.getNickname(), ch->second.getName()));
					continue; // Skip to the next channel
				}

				// 10. Too many members
				if (ch->second.countMembers() >= ch->second.getUserLimit() && ch->second.getUserLimit() != 0)
				{
					logger.info("[JOIN] :: Channel : " + channelName + " is full (" + Utils::intToString(ch->second.countMembers()) + ")");
					message.sendToClient(client.getFd(), ERR_CHANNELISFULL(client.getNickname(), ch->second.getName()));
					continue; // Skip to the next channel
				}
    		}
			// 11. Send join msg
			//message.sendToChannel(channelName, RPL_JOINMSG(client.getNickname(), client.getHostname(), channelName), channels);

			// 9. Send hexchat client msg
			if (!ch->second.isMember(client.getFd()))
			{
				message.sendToClient(client.getFd(), msg_text);
				msg_text = hx_join_format(channelName, client, true);
				logger.info("[JOIN] :: HC member join message: " + msg_text);
				message.sendToChannel(channelName, msg_text, channels, -1);
			}
			
			// 10. Show history (if channel is new there will be no history)
			//showChannelHistory(it->second.getHistory(), client.getFd());

			// 12. Add clients to client's channel and channel's members
    		client.addChannel(channelName);
    		ch->second.addMember(client.getFd(), &client);
		}
	}
	else	// NEED MORE PARAMS
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
}

bool	IRCServer::isChannelNameValid(const std::string& channelName)
{
    if (channelName.empty() || channelName.length() > CHANNELLEN) {
        return false;
    }

    for (size_t i = 1; i < channelName.length(); ++i) {
        char c = channelName[i];
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']' && c != '{' && c != '}' && c != '|' && c != '#' && c != '&') {
            return false;
        }
        if (c == ' ' || c == ',') {
            return false;
        }
    }
    return true;
}

void	IRCServer::showChannelHistory(const std::vector<std::string> history, int fd)
{
	for (std::vector<std::string>::const_iterator it = history.begin(); it != history.end(); it++)
	{
		const std::string& msg_text = *it;
		message.sendToClient(fd, msg_text);
	}
}