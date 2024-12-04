#include "IRCServer.hpp"

/* HEXCLIENT MSG FORMAT */
std::string	IRCServer::hx_privmsg_format(const std::string& command, Client& sender)
{
	//"@time=2024-12-03T01:14:26.669Z :Nick!~USER@host CMD ..."
	std::string message;

	// TIME
	message = "@time=" + Utils::getCurrentTimeISO8601();

	// NICK SENDER
	message += " :" + sender.getNickname() + "!";

	// USER SENDER
	message += "~" + sender.getUsername() + "@" + sender.getHostname() + " ";

	// COMMAND
	message += command + CRLF;

	return (message);
}

std::string	IRCServer::hx_join_format(const std::string& command, Client& sender)
{
	//"@time=2024-12-04T17:10:52.472Z :webo!~A@89.131.139.38 JOIN #dos * :realname\r\n";
	std::string message;

	// TIME
	message = "@time=" + Utils::getCurrentTimeISO8601();

	// NICK SENDER
	message += " :" + sender.getNickname() + "!";

	// USER SENDER
	message += "~" + sender.getUsername() + "@" + sender.getHostname() + " ";

	// COMMAND
	message += command;
	
	message += " :realname\r\n";

	return (message);
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

		// 3. Format msg (hexchat)
		std::string	msg_text = hx_privmsg_format(command, client);
		logger.info("[PRIVMSG] :: Sending message: " + msg_text);

		// 4. Send msg to each destinatary
		for (std::vector<std::string>::iterator it = destinataries.begin(); it != destinataries.end(); ++it)
		{
			const std::string& dest_nick = *it;
			int dest_fd = -1;

			// 5. Destinatary is a channel
			if (dest_nick.size() > 1 && dest_nick[0] == '#')
			{
				std::string realName = Utils::removeLeadingChar(dest_nick, '#');
				if (!existsChannelByName(realName))		// does not exist
				{
					logger.warning("[PRIVMSG] Channel '" + realName + " not found.");
					message.sendToClient(client.getFd(), ERR_CHANNELNOTFOUND(client.getNickname(), dest_nick));
					continue;
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
					message.sendToClient(client.getFd(), ERR_NOSUCHNICK(client.getNickname()));
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
	logger.info("[JOIN] :: User " + client.getNickname() + " joining a channel.");

	// 1. Splitting cmd
	std::vector<std::string> split_command = Utils::splitBySpaces(command);
	int	command_len = split_command.size();

	if (command_len >= 2)
	{	
		// 2. Extracting channels & passwords
		std::vector<std::string> cmd_channels = Utils::split(split_command[1], ",");
		std::vector<std::string> cmd_keys;
		if (command_len == 3)
			cmd_keys = Utils::split(split_command[2], ",");
		
		// 3. Format msg (hexchat)
		std::string msg_text = hx_privmsg_format(command, client);
		logger.info("[JOIN] :: HC Message: " + msg_text);
		
		// 4. Join each channel (if possible) 
		for (size_t i = 0; i < cmd_channels.size(); ++i)
		{	
			// 5. Get channel name and key
			std::string channel_name = Utils::removeLeadingChar(cmd_channels[i], '#');
    		std::string channel_key = (i < cmd_keys.size()) ? cmd_keys[i] : "";

    		// 6. Search in existing channels
    		std::map<std::string, Channel>::iterator it = channels.find(channel_name);
			// 7. Channel does not exist
			if (it == channels.end())
			{ 
				logger.info("[JOIN] :: Creating channel : " + channel_name + " " + channel_key);
				if (!channel_key.empty())	// PASSWD
					channels.insert(std::make_pair(channel_name, Channel(channel_name, channel_key)));
				else						// NO PASSWD
					channels.insert(std::make_pair(channel_name, Channel(channel_name)));

				it = channels.find(channel_name);

				// 8. User becomes operator
				it->second.addOperator(client.getFd(), &client);				
    		} 
			// 7. Channel does exist
			else
			{
				// 8. Wrong key entered
				if (!it->second.getChannelKey().empty() && (channel_key.empty() || it->second.getChannelKey() != channel_key)) {
					logger.info("[JOIN] :: Wrong channel key : " + channel_name + ". Entered key: " + channel_key + ", Actual key: " + it->second.getChannelKey());
					message.sendToClient(client.getFd(), ERR_BADCHANNELKEY(client.getNickname(), it->second.getName()));
					continue; // Skip to the next channel
				}
    		}
			// 11. Send join msg
			message.sendToChannel(channel_name, RPL_JOINMSG(client.getNickname(), client.getHostname(), cmd_channels[0]), channels);

			// 9. Send hexchat client msg
			if (!it->second.isMember(client.getFd()))
			{
				message.sendToClient(client.getFd(), msg_text);
				msg_text = hx_join_format(command, client);
				logger.info("[JOIN] :: HC member join message: " + msg_text);
				message.sendToChannel(channel_name, msg_text, channels, -1);
			}
			
			// 10. Show history (if channel is new there will be no history)
			//showChannelHistory(it->second.getHistory(), client.getFd());

			// 12. Add clients to client's channel and channel's members
    		client.addChannel(channel_name);
    		it->second.addMember(client.getFd(), &client);
		}
	}
	else	// NEED MORE PARAMS
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
}

void	IRCServer::showChannelHistory(const std::vector<std::string> history, int fd)
{
	for (std::vector<std::string>::const_iterator it = history.begin(); it != history.end(); it++)
	{
		const std::string& msg_text = *it;
		message.sendToClient(fd, msg_text);
	}
}