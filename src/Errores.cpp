#include "../inc/Errores.hpp"

void Error::mensaje_error_conexion() {
	std::cerr << "Error: No se pudo establecer la conexión." << std::endl;
}

void Error::mensaje_error_comando() {
	std::cerr << "Error: Comando no reconocido." << std::endl;
}
