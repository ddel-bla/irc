#include "IRCServer.hpp"

bool	areArgsValid(std::string& port, std::string& passwd)
{
	if (!(std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535))
		return (false);

	std::vector<std::string> split_passwd = Utils::splitBySpaces(passwd);
	if (split_passwd.size() > 1 || std::strlen(passwd.c_str()) > 15)
		return (false);

	return (true);
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <passwd>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string port = argv[1];
    std::string password = argv[2];

    if (!areArgsValid(port, password)) {
        std::cerr << "Error: Invalid arguments." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        Servidor servidor(std::atoi(port.c_str()), password);
        if (!servidor.iniciar_servidor()) {
            std::cerr << "Error initializing server..." << std::endl;
            return EXIT_FAILURE;
        }

        servidor.ejecutar();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
