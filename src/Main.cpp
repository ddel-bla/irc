#include "IRCServer.hpp"

bool areArgsValid(std::string& port, std::string& passwd) {
    if (!(std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535))
        return false;

    if (std::strlen(passwd.c_str()) > 15)
        return false;

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string port = argv[1];
    std::string password = argv[2];

    if (!areArgsValid(port, password)) {
        std::cerr << "Error: Invalid arguments." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        IRCServer server(std::atoi(port.c_str()), password);
        if (!server.startServer()) {
            std::cerr << "Error initializing server..." << std::endl;
            return EXIT_FAILURE;
        }

        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}