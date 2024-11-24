#ifndef CANAL_HPP
#define CANAL_HPP

#include <map>
#include "Cliente.hpp"

class Canal {
	public:
		std::string nombre; // Nombre del canal
		std::string topic;  // Tópico del canal 
		std::map<int, Cliente*> miembros; // FD del cliente -> Puntero al cliente

		// Constructor
		Canal(const std::string& nombre);

		// Métodos para gestionar miembros
		void agregarMiembro(int cliente_fd, Cliente* cliente);
		void eliminarMiembro(int cliente_fd);
		bool esMiembro(int cliente_fd) const;
		void enviarMensaje(const std::string& mensaje) const;

		// Obtener información del canal
		size_t cantidadMiembros() const;
};

#endif
