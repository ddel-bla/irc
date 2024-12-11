#include "IRCServer.hpp"

void    IRCServer::kick(const std::string& command, Client& client)
{
    logger.info("[KICK] :: " + client.getNickname() + " wants to kick a user from a channel");

    // 1. Splitting message
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();
    
    if (command_len >= 3)
    {
        // 2. Extract channel
        std::string channelName = Utils::removeLeadingChar(split_command[1], '#');
        std::map<std::string, Channel>::iterator channel = channels.find(channelName);
        if (channel == channels.end())  // Channel does not exists
        {
            logger.warning("[KICK] :: Channel does not exist : " + channelName + ".");
            message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
            return ;
        }

        // 3. Check if client (kicking) is in the channel
        if (!channel->second.isMember(client.getFd()))
        {
            logger.warning("[KICK] :: Client : " + client.getNickname() + " is not in channel '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
            return ;
        }

        // 4. Check if client (kicking) has the required permissions
        if (!channel->second.isOperator(client.getFd()))
        {
            logger.warning("[KICK] :: Client : " + client.getNickname() + " is not a channel operator in '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
            return ;
        }

        // 5. Extract kicked clients
        std::vector<std::string> kick_clients = Utils::split(split_command[2], ",");
        if (kick_clients.size() > MAXTARGETS) // Limit kicked clients
        {
            logger.warning("[KICK] " + client.getNickname() + " selected too many targets (" + Utils::intToString(kick_clients.size()) + ")");
			message.sendToClient(client.getFd(), ERR_TOOMANYTARGETS(client.getNickname()));
			return;
        }

        // 6. Send kick to each client
        for (std::vector<std::string>::iterator it = kick_clients.begin(); it != kick_clients.end(); ++it)
        {
            std::string kicked_name = *it;
            int fd = findFdByNickname(kicked_name);
            std::map<int, Client*>::iterator k_client = clients.find(fd);

            // 7. Check if kicked user is in channel
            if (!channel->second.isMember(fd))
            {
                logger.warning("[KICK] :: Client : " + client.getNickname() + " wants to kick " + kicked_name + " from " + channelName + " who is not on the channel.");
                message.sendToClient(client.getFd(), ERR_USERNOTINCHANNEL(client.getNickname(), kicked_name, channelName));
                return ;
            }

            // 8. Get reason of kick
            std::string reason = getKickReason(command, command_len);
            
            // 9. Format msg (hexchat)
            std::string msg_text = hx_quit_format(channelName, client, kicked_name, reason);
            logger.info("[KICK] :: HC Message: " + msg_text);
            
            // 10. Send message
            message.sendToChannel(channelName, msg_text, channels);

            // 11. Delete channels
            channel->second.removeMember(fd);
            channel->second.removeOperator(fd);
            k_client->second->removeChannel(channelName);
        } 
    }
    else        // NOT ENOUGH PARAMS
    {
        logger.warning("[KICK] " + client.getNickname() + " --> Not enough params.");
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
    }
}

std::string IRCServer::getKickReason(const std::string& command, int command_len)
{
    std::string         reason;
    (void) command;

    if (command_len > 3)
        reason = Utils::getMessageWithoutPrefixes(command, 3);
    else
        reason = " You dont belong here! -.-";
    return (reason);
}

void    IRCServer::invite(const std::string& command, Client& client)
{
    logger.info("[INVITE]:: User " + client.getNickname() + " is inviting...");

    // 1. Splitting cmd
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();

    if (command_len == 3)
    {
        // 2. Extract user and channel
        std::string user = split_command[1];
        std::string channelName = Utils::removeLeadingChar(split_command[2], '#');
        
        // 3. Channel not exists
        std::map<std::string, Channel>::iterator ch = channels.find(channelName);
        if (ch == channels.end())
        {
            logger.warning("[INVITE] :: Channel does not exist : " + channelName + ".");
            message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
            return ;
        }

        // 4. Channel not intive-only mode
        if (!ch->second.isInviteOnly())
        {
            logger.warning("[INVITE] :: Channel : " + channelName + " has not invite-only mode.");
            message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
            return ;
        }

        // 5. User has no privileges
        if (!ch->second.isMember(client.getFd()))
        {
            logger.warning("[INVITE] :: Client : " + client.getNickname() + " is not in channel '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
            return ;   
        }

        // 6. Is invited user already on channel ?
        int invited_fd = findFdByNickname(user);
        std::map<int, Client*>::iterator invited_user = clients.find(invited_fd);
        if (ch->second.isMember(invited_fd))
        {
            logger.warning("[INVITE] :: Client : " + client.getNickname() + " is already in channel '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_USERONCHANNEL(client.getNickname(), user, channelName));
            return ;
        }

        // 7. Add user to inviting list
        ch->second.addInvited(invited_fd, invited_user->second);

        // 8. Send client text (hxc)D
        std::string msg_text = hx_generic_format(command, client);
        message.sendToClient(invited_fd, msg_text);

        // 9. Invited message
        message.sendToClient(client.getFd(), RPL_INVITING(client.getNickname(), invited_user->second->getNickname(), channelName));
    }
    else if (command_len > 3)   // NOT ENOGH PARAMS
    {
        logger.warning("[INVITE] " + client.getNickname() + " --> Not enough params.");
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
    }
    else                        // TOO MANY PARAMS
    {
       logger.warning("[INVITE] " + client.getNickname() + " --> Too many params.");
		message.sendToClient(client.getFd(), ERR_TOOMANYPARAMS(client.getNickname(), command)); 
    } 
}

void    IRCServer::topic(const std::string& command, Client& client)
{
    logger.info("[TOPIC]:: User " + client.getNickname() + " changing topic...");

    // 1. Splitting cmd
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();

    if (command_len >= 2)
    {   
        // 2. Extract user and channel
        std::string channelName = Utils::removeLeadingChar(split_command[1], '#');
        std::string topic;
        if (command_len >= 3)
            topic = Utils::getMessageWithoutPrefixes(command, 2);
            
        // 3. Channel not exists
        std::map<std::string, Channel>::iterator ch = channels.find(channelName);
        if (ch == channels.end())
        {
            logger.warning("[TOPIC] :: Channel does not exist : " + channelName + ".");
            message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
            return ;
        }

        // 4. User is not on channel
        if (!ch->second.isMember(client.getFd()))
        {
            logger.warning("[TOPIC] :: Client : " + client.getNickname() + " is not in channel '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
            return ;   
        }

        // 5. User has no privileges if protected-topic enabled
        if (ch->second.isTopicRestricted() && !ch->second.isOperator(client.getFd()))
        {
            logger.warning("[TOPIC] :: Client : " + client.getNickname() + " is not a channel operator in '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
            return ;
        }
        
        // 6. No topic
        if (command_len == 2)
        {
            if (ch->second.getTopic().empty())
            {
                logger.info("[TOPIC] :: There is no topic to display.");
                message.sendToClient(client.getFd(), RPL_NOTOPIC(client.getNickname(), channelName));
                return ;
            }
            else
            {
                logger.info("[TOPIC] :: The topic of '" + ch->second.getName() + "' is: " + ch->second.getTopic() + ".");
                message.sendToClient(client.getFd(), RPL_TOPIC(client.getNickname(), channelName, ch->second.getTopic()));
                return ;
            }
        }
        // 7. Set topic
        if (!topic.empty())
        {
            ch->second.setTopic(topic);
            logger.info("[TOPIC] :: The topic of '" + ch->second.getName() + "' is: " + ch->second.getTopic() + ".");
            message.sendToChannel(channelName, RPL_TOPIC(client.getNickname(), channelName, ch->second.getTopic()), channels);
        }
        else
        {
            ch->second.setTopic("");
            logger.info("[TOPIC] :: There is no topic to display.");
            message.sendToChannel(channelName, RPL_NOTOPIC(client.getNickname(), channelName), channels);
        }
    }
    else if (command_len < 2)   // NOT ENOGH PARAMS
    {
        logger.warning("[TOPIC] " + client.getNickname() + " --> Not enough params.");
        std::string channelName = Utils::removeLeadingChar(split_command[1], '#');
		
        message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
    }
}

void    IRCServer::mode(const std::string& command, Client& client)
{
    logger.info("[MODE]:: User " + client.getNickname() + " is changing mode...");

    // 1. Splitting cmd
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();

    if (command_len >= 2)
    {
        // 2. Extract user and channel
        std::string channelName = Utils::removeLeadingChar(split_command[1], '#');
        std::string modes;                  // Chanel modes (+i)
        std::vector<std::string> params;    // Mode params 
        
        if (command_len >= 3)
        {
            modes = split_command[2];
            params = Utils::splitBySpaces(Utils::getMessageWithoutPrefixes(command, 3));
        }
        
        // 3. Channel not exists
        std::map<std::string, Channel>::iterator ch = channels.find(channelName);
        if (ch == channels.end())
        {
            logger.warning("[MODE] :: Channel does not exist : " + channelName + ".");
            message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
            return ;
        }

        // 4. No modes given
        if (command_len == 2)
        {
            logger.warning("[MODE] :: No modes were giving (displaying channel '" + channelName + "' modes).");
            sendRplISupport(client.getFd(), client.getNickname());
            return ;
        }
        
        // 4. User is not on channel
        if (!ch->second.isMember(client.getFd()))
        {
            logger.warning("[MODE] :: Client : " + client.getNickname() + " is not in channel '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
            return ;   
        }

        // 5. User has no privileges if protected-topic enabled
        if (!ch->second.isOperator(client.getFd()))
        {
            logger.warning("[MODE] :: Client : " + client.getNickname() + " is not a channel operator in '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
            return ;
        }

        // 7. Set modes
        setModes(ch->second, modes, params, client);
    }
    else if (command_len < 2)   // NOT ENOGH PARAMS
    {
        logger.warning("[MODE] " + client.getNickname() + " --> Not enough params.");
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
    }
}

void IRCServer::setModes(Channel& ch, std::string& modes, std::vector<std::string> params, Client& sender)
{
    size_t paramIndex = 0;
    bool addMode = true;

    for (size_t i = 0; i < modes.length(); ++i) {
        char modeChar = modes[i];

        // 1. Add mode
        if (modeChar == '+')
            addMode = true;
        // 2. Remove mode
        else if (modeChar == '-')
            addMode = false;
        else {
            
            // 3. Process each mode
            std::string param;
            if ((modeChar == 'k' || modeChar == 'l' || modeChar == 'o') && paramIndex < params.size()) {
                param = params[paramIndex++]; // Get next param if necessary
            }

            switch (modeChar) {
                case 'i': // Invite-only
                    ch.iMode(addMode);
                    break;

                case 't': // Topic restriction
                    ch.tMode(addMode);
                    break;

                case 'k': // Channel key
                    if (!ch.kMode(param, addMode))
                    {
                        logger.warning("[MODE] Invalid key '" + param + "'.");
                        message.sendToClient(sender.getFd(), ERR_INVALIDKEY(sender.getNickname(), ch.getName()));
                        continue;
                    }
                    break;

                case 'l': // User limit
                    if (!ch.lMode(param, addMode))
                    {
                        logger.warning("[MODE] Invalid limit '" + param + "'.");
                        message.sendToClient(sender.getFd(), ERR_INVALIDLIMIT(sender.getNickname(), ch.getName()));
                        continue;
                    }
                    break;

                case 'o': // Operator privilege
                {
                    int fd = findFdByNickname(param);
                    std::map<int, Client*>::iterator user = clients.find(fd);
                    if (user == clients.end())
                    {
                        logger.warning("[MODE] :: Client : " + param + " does not exist.");
                        message.sendToClient(sender.getFd(), ERR_NOSUCHNICK(param));
                        continue;
                    }
                    else if (!ch.isMember(user->second->getFd()))
                    {
                        logger.warning("[MODE] :: Client : " + user->second->getNickname() + " is not in channel '" + ch.getName() + "'.");
                        message.sendToClient(sender.getFd(), ERR_USERNOTINCHANNEL(sender.getNickname(), user->second->getNickname(), ch.getName()));
                        continue;
                    }
                    else if (!ch.oMode(user->second, addMode))
                    {
                        logger.warning("[MODE] :: Client : " + user->second->getNickname() + " is not an operator '" + ch.getName() + "'.");
                        message.sendToClient(sender.getFd(), ERR_NOTANOPERATOR(sender.getNickname(), ch.getName()));
                        continue;
                    }
                    break;
                }
                default:
                    logger.warning("[MODE] Invalid mode '" + std::string(1, modeChar) + "'.");
                    message.sendToClient(sender.getFd(), ERR_INVALIDMODEPARAM(sender.getNickname(), ch.getName(), modeChar));
                    continue;
            }
            std::string msg_text = hx_mode_format(ch.getName(), sender, addMode, modeChar, param);
            logger.info("[MODE] :: HC member mode message: " + msg_text);
            message.sendToChannel(ch.getName(), msg_text, channels);
        }   
    }
}

void IRCServer::who(const std::string& command, Client& client)
{
    logger.info("[WHO] :: User " + client.getNickname() + " requested WHO information.");

    // 1. Split command
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();

    if (command_len >= 2)
    {
        // 2. Extract channel
        std::string channelName = Utils::removeLeadingChar(split_command[1], '#');
        std::map<std::string, Channel>::iterator ch = channels.find(channelName);

        // 3. Channel does not exist
        if (ch == channels.end()) 
        {
            logger.warning("[WHO] :: Channel does not exist: " + channelName + ".");
            message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
            return;
        }

        // 4. User not on channel
        if (!ch->second.isMember(client.getFd())) 
        {
            logger.warning("[WHO] :: User " + client.getNickname() + " is not in channel '" + channelName + "'.");
            message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
            return;
        }
    
        // 5. Msg obtain users
        logger.info("[WHO] :: Listing users in channel " + channelName + ".");
        std::string msg_text = hx_who_format(channelName, client, ch->second.getMembers());
        message.sendToClient(client.getFd(), msg_text);

        // 6. Msg end of list
        message.sendToClient(client.getFd(), RPL_ENDOFNAMES(client.getNickname(), channelName));
    }
    else if (command_len < 2)   // NOT ENOUGH PARAMS
    {
        logger.warning("[WHO] :: Not enough parameters.");
        message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
        return;
    }
}

void IRCServer::part(const std::string& command, Client& client)
{
    logger.info("[PART]:: User " + client.getNickname() + " is attempting to leave channel(s)...");

    // 1. Split commands
    std::vector<std::string> split_command = Utils::splitBySpaces(command);
    int command_len = split_command.size();
    
    if (command_len > 2)
    {
        // 3. Extract channels
        std::vector<std::string> channelNames = Utils::split(split_command[1], ",");

        
        for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); ++it)
        {
            const std::string& channelNameRaw = *it;

            // 4. Get channel name
            std::string channelName = Utils::removeLeadingChar(channelNameRaw, '#');

            // 5. Channel does not exist
            std::map<std::string, Channel>::iterator channelIt = channels.find(channelName);
            if (channelIt == channels.end())
            {
                logger.warning("[PART] :: Channel does not exist: " + channelName);
                message.sendToClient(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
                continue;
            }

            // 6. Client not member
            Channel& channel = channelIt->second;
            if (!channel.isMember(client.getFd()))
            {
                logger.warning("[PART] :: Client " + client.getNickname() + " is not in channel: " + channelName);
                message.sendToClient(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
                continue;
            }

            // 7. Remove the client from the channel
            channel.removeMember(client.getFd()); // todo

            // 8. Notify other channel members of the departure
            std::string msg_text = hx_part_format(channel.getName(), client);
            message.sendToChannel(channel.getName(), msg_text, channels, client.getFd());

            // 9. Send confirmation to the client
            message.sendToClient(client.getFd(), msg_text);

            logger.info("[PART] :: Client " + client.getNickname() + " has left channel: " + channelName);
        }
    }
    else if (command_len < 2)
    {
        logger.warning("[PART] :: Not enough parameters provided by " + client.getNickname());
        message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
        return;
    }
}
