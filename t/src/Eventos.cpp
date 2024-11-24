#include "Eventos.hpp"

void Evento::evento_conexión() {
	std::cout << "Un nuevo usuario se ha conectado." << std::endl;
}

void Evento::evento_desconexión() {
	std::cout << "Un usuario se ha desconectado." << std::endl;
}
