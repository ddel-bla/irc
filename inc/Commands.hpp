#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <iostream>

class Command {
	public:
		void processCommand(const std::string& command, const std::string& arguments);
};

#endif
