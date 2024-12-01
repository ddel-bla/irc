#include "Commands.hpp"

void Command::processCommand(const std::string& command, const std::string& arguments) {
	if (command == "/nick") {
		std::cout << "Changing nickname to: " << arguments << std::endl;
	} else if (command == "/user") {
		std::cout << "Registering user: " << arguments << std::endl;
	} else if (command == "/quit") {
		std::cout << "Disconnecting user." << std::endl;
	} else {
		std::cerr << "Unrecognized command." << std::endl;
	}
}
