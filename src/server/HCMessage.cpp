#include "IRCServer.hpp"

/* -- HEXCLIENT MSG FORMAT -- */

/* GENERIC FORMAT */
std::string	IRCServer::hx_generic_format(const std::string& command, Client& sender)
{
	//"@time=2024-12-03T01:14:26.669Z :Nick!~USER@host CMD ..."
	std::string message;

	// TIME
	message = "@time=" + Utils::getCurrentTimeISO8601();

	// NICK SENDER
	message += " :" + sender.getNickname() + "!";

	// USER SENDER
	message += "~" + sender.getUsername() + "@" + sender.getHostname() + " ";

	// To upper command word (privmsg clau :hola)
	std::string	cmd_formatted = Utils::capitalizeFirstWord(command);

	// COMMAND
	message += cmd_formatted + CRLF;

	return (message);
}

/* JOIN CMD FORMAT */
std::string	IRCServer::hx_join_format(const std::string& channel_name, Client& sender, bool member_joined)
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
	message += "JOIN #" + channel_name;
	
	if (member_joined)
		message += " * :realname\r\n";
	else
		message += CRLF;

	return (message);
}

/* SEVERAL DESTINATARIES */
std::string	IRCServer::hx_quit_format(const std::string& channel_name, Client& sender, const std::string& kicked_user, std::string& reason)
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
	message += "KICK #" + channel_name;

	message += " " + kicked_user + "" + reason  + CRLF;

	return (message);
}

/* MODE CMD */
std::string IRCServer::hx_mode_format(const std::string& channelName, Client& sender, bool addMode, char modeChar, std::string& param)
{
	//"@time=2024-12-04T17:10:52.472Z :webo!~A@89.131.139.38 JOIN #dos * :realname\r\n";
	std::string message;
	std::string command = "+i";

	// TIME
	message = "@time=" + Utils::getCurrentTimeISO8601();

	// NICK SENDER
	message += " :" + sender.getNickname() + "!";

	// USER SENDER
	message += "~" + sender.getUsername() + "@" + sender.getHostname() + " ";

	// COMMAND
	message += "MODE #" + channelName + " ";

	std::string result;

	// '+' or '-'
	result += (addMode ? '+' : '-');

	// Actual mode (modeChar)
	result += modeChar;

	// Add param
	if (!param.empty())
		result += " " + param;
	
	// merge
	message += result  + CRLF;

	return (message);
}