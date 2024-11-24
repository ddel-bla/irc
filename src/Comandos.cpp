#include "Comandos.hpp"

void Comando::procesar_comando(const std::string& comando, const std::string& argumentos) {
	if (comando == "/nick")
		std::cout << "Cambiando nickname a: " << argumentos << std::endl;
	else if (comando == "/user")
		std::cout << "Registrando usuario: " << argumentos << std::endl;
	else if (comando == "/quit")
		std::cout << "Desconectando usuario." << std::endl;
	else
		std::cerr << "Comando no reconocido." << std::endl;
}
