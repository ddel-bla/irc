#include "../inc/Comando.hpp"
#include <sstream>
#include <iostream>

void Comando::procesarComando(const std::string& entrada, int cliente_fd,
                              std::map<int, Cliente*>& clientes,
                              std::map<std::string, Canal>& canales,
                              Evento& manejadorEventos) {
    if (canales.empty()) {
        std::cout << "Actualmente no hay canales creados." << std::endl;
    }

    if (entrada == "/broadcast") {
        manejadorEventos.enviarMensajeGlobal("Mensaje global de " + clientes[cliente_fd]->getNickname(), clientes);
    } else {
        manejadorEventos.enviarMensajeCliente(cliente_fd, "Comando no reconocido.\n");
    }
}
