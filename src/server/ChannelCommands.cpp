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