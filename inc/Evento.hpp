#ifndef EVENTO_HPP
#define EVENTO_HPP

#include <string>
#include <map>
#include <vector>
#include "Cliente.hpp"
#include "Canal.hpp"

class Evento {
	private:
		std::vector<std::string> historialEventos;

	public:
		Evento();

		void enviarMensajeGlobal(const std::string& mensaje, const std::map<int, Cliente*>& clientes);
		void enviarMensajeCliente(int cliente_fd, const std::string& mensaje, const std::map<int, Cliente*>& clientes);
		void enviarMensajeCanal(const std::string& canal, const std::string& mensaje, const std::map<std::string, Canal>& canales);

		void notificarConexion(int cliente_fd, const std::string& nickname, const std::map<int, Cliente*>& clientes);
		void notificarDesconexion(int cliente_fd, const std::string& nickname, const std::map<int, Cliente*>& clientes);

		void registrarEvento(const std::string& tipoEvento, const std::string& detalle);
		const std::vector<std::string>& obtenerHistorial() const;
};

#endif
