#include "IRCServer.hpp"

/* PRIVMSG */
void IRCServer::privmsg(const std::string& command, Client& client)
{
	std::vector<std::string> split_command = Utils::splitBySpaces(command);
	int	command_len = split_command.size();

	std::vector<std::string>	destinataries;
	std::string 				msg_text;

	if (command_len >= 3)
	{
		// SKIP 'NICK' and EXTRACT DESTS
		destinataries = Utils::split(split_command[1], ",");
		std::cout << "Destinatarios: " << destinataries[0] << std::endl;

		// FORMAT MSG
		msg_text = format_privmsg(command, client);
		std::cout << "Mensaje: " << msg_text << std::endl;
		
		// DESTS ITERATION
		for (std::vector<std::string>::iterator it = destinataries.begin(); it != destinataries.end(); ++it)
		{
			const std::string& dest_nick = *it;
			int dest_fd = -1;

			if (dest_nick.size() > 1 && dest_nick[0] == '#') // CHANNEL
			{
				std::cout << "Es un canal: " << dest_nick << std::endl;
				// Aquí podrías manejar el envío del mensaje al canal
				// Por ejemplo: enviarMensajeAlCanal(dest_nick, msg, client);
				// Se ha de guardar
			}
			else	// USER
			{
				dest_fd = findFdByNickname(dest_nick);
				if (dest_fd == -1) {
					message.sendToClient(client.getFd(), ERR_NOSUCHNICK(client.getNickname()));
					continue;
				}
				message.sendToClient(dest_fd, msg_text);
			}
    	}
	}
	else if (command_len == 2) 	// NO MSG
		message.sendToClient(client.getFd(), ERR_NOTEXTTOSEND(client.getNickname()));
	else						// NO DESTINATARY
		message.sendToClient(client.getFd(), ERR_NORECIPIENT(client.getNickname(), command));
}

std::string	IRCServer::format_privmsg(const std::string& command, Client& sender)
{
	//"@time=2024-12-03T01:14:26.669Z :Nick!~mailto:USER@host PRIVMSG webo :a"
	std::string message;

	// TIME
	message = "@time=" + Utils::getCurrentTimeISO8601();

	// NICK SENDER
	message += " :" + sender.getNickname() + "!";

	// USER SENDER
	message += "~mailto:" + sender.getUsername() + "@" + sender.getHostname() + " ";

	message += command + CRLF;
	return (message);
}

int IRCServer::findFdByNickname(const std::string& nickname) {
	
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it->second->getNickname() == nickname) {
			return it->first;
		}
	}

	return -1;
}

/* JOIN CHANNEL */
void	IRCServer::join(const std::string& command, Client& client)
{
	std::vector<std::string> split_command;
	int	command_len;
	std::vector<std::string>	cmd_channels;
	std::vector<std::string>	cmd_keys;
	std::map<std::string, Channel>::iterator it;

	split_command = Utils::splitBySpaces(command);
	command_len = split_command.size();
	if (command_len >= 2)
	{	
		// CHANNELS, PASSWDS
		cmd_channels = Utils::split(split_command[1], ",");
		if (command_len == 3)
			cmd_keys = Utils::split(split_command[2], ",");
		
		it = channels.find(cmd_channels[0]); // FOR LOOP
		if (it == channels.end())
		{
			// El canal no existe, se inserta usando insert
			std::cout << "Insertando canal.." << std::endl;
			if (cmd_keys.size() > 0)
				channels.insert(std::make_pair(cmd_channels[0], Channel(cmd_channels[0], cmd_keys[0])));
			else
				channels.insert(std::make_pair(cmd_channels[0], Channel(cmd_channels[0])));
			it = channels.find(cmd_channels[0]);
		}
		else
		{
			if (it->second.getChannelKey() == cmd_keys[0])
				std::cout << "Correct passwd" << std::endl;
			else
				message.sendToClient(client.getFd(), ERR_BADCHANNELKEY(client.getNickname(), it->second.getName()));
				
		}
		//Agrega el cliente a LOS canales
		it->second.addMember(client.getFd(), &client);
		
		std::string join_message = client.getNickname() + " has joined " + cmd_channels[0] + "\n";
		
		message.sendToClient(client.getFd(), join_message);
	}
	else
		message.sendToClient(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname()));
}