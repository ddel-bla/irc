#include "../inc/Evento.hpp"
#include <unistd.h>
#include <iostream>

Evento::Evento() {
    historialEventos.clear();
}

void Evento::enviarMensajeGlobal(const std::string& mensaje, const std::map<int, Cliente*>& clientes) {
    for (const auto& par : clientes) {
        send(par.first, mensaje.c_str(), mensaje.size(), 0);
    }
    registrarEvento("Global", mensaje);
}

void Evento::enviarMensajeCliente(int cliente_fd, const std::string& mensaje) {
    send(cliente_fd, mensaje.c_str(), mensaje.size(), 0);
    registrarEvento("Cliente", "FD " + std::to_string(cliente_fd) + ": " + mensaje);
}

void Evento::enviarMensajeCanal(const std::string& canal, const std::string& mensaje, const std::map<std::string, Canal>& canales) {
    if (canales.find(canal) != canales.end()) {
        for (const auto& miembro : canales.at(canal).miembros) {
            send(miembro.first, mensaje.c_str(), mensaje.size(), 0);
        }
        registrarEvento("Canal", "Canal " + canal + ": " + mensaje);
    }
}

void Evento::notificarConexion(int cliente_fd, const std::string& nickname, const std::map<int, Cliente*>& clientes) {
    std::string mensaje = nickname + " se ha conectado.";
    enviarMensajeGlobal(mensaje, clientes);
}

void Evento::notificarDesconexion(int cliente_fd, const std::string& nickname, const std::map<int, Cliente*>& clientes) {
    std::string mensaje = nickname + " se ha desconectado.";
    enviarMensajeGlobal(mensaje, clientes);
}

void Evento::registrarEvento(const std::string& tipoEvento, const std::string& detalle) {
    historialEventos.push_back("[" + tipoEvento + "] " + detalle);
}

const std::vector<std::string>& Evento::obtenerHistorial() const {
    return historialEventos;
}
