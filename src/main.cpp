#include "IRCServer.hpp"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Uso: ./ircserv <puerto> <contraseña>" << std::endl;
		return 1;
	}
	// to do validar puerto y la contraseña no hace nada
	int puerto = std::atoi(argv[1]);
	std::string password = argv[2];
	
	Servidor servidor(puerto, password);
	if (!servidor.iniciar_servidor()) {
		std::cerr << "Error al iniciar el servidor." << std::endl;
		return 1;
	}
	servidor.ejecutar();
	return 0;
}
