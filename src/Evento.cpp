#include "Evento.hpp"
#include <iostream>
#include <sstream> // Para std::ostringstream

// Constructor
Evento::Evento() {}

// Envía un mensaje a todos los clientes conectados
void Evento::enviarMensajeGlobal(const std::string& mensaje, const std::map<int, Cliente*>& clientes) {
    std::map<int, Cliente*>::const_iterator it;
    for (it = clientes.begin(); it != clientes.end(); ++it) {
        if (it->second) {
            it->second->enviarMensaje(mensaje);
        }
    }
    registrarEvento("MensajeGlobal", "Mensaje enviado: " + mensaje);
}

// Envía un mensaje a un cliente específico
void Evento::enviarMensajeCliente(int cliente_fd, const std::string& mensaje, const std::map<int, Cliente*>& clientes) {
    std::map<int, Cliente*>::const_iterator it = clientes.find(cliente_fd);
    if (it != clientes.end() && it->second) {
        it->second->enviarMensaje(mensaje);
        std::ostringstream log;
        log << "FD " << cliente_fd << ": " << mensaje;
        registrarEvento("MensajeCliente", log.str());
    } else {
        std::cerr << "Error: Cliente no encontrado para FD " << cliente_fd << std::endl;
    }
}

// Envía un mensaje a un canal
void Evento::enviarMensajeCanal(const std::string& canal, const std::string& mensaje, const std::map<std::string, Canal>& canales) {
    std::map<std::string, Canal>::const_iterator it = canales.find(canal);
    if (it != canales.end()) {
        it->second.enviarMensaje(mensaje);
        registrarEvento("MensajeCanal", "Canal " + canal + ": " + mensaje);
    } else {
        std::cerr << "Error: Canal no encontrado: " << canal << std::endl;
    }
}

// Notifica la conexión de un cliente
void Evento::notificarConexion(int cliente_fd, const std::string& nickname, const std::map<int, Cliente*>& clientes) {
    std::string mensaje = "Usuario " + nickname + " se ha conectado.";
    enviarMensajeGlobal(mensaje, clientes);

    std::ostringstream log;
    log << "FD " << cliente_fd << " - Nickname: " << nickname;
    registrarEvento("Conexion", log.str());
}

// Notifica la desconexión de un cliente
void Evento::notificarDesconexion(int cliente_fd, const std::string& nickname, const std::map<int, Cliente*>& clientes) {
    std::string mensaje = "Usuario " + nickname + " se ha desconectado.";
    enviarMensajeGlobal(mensaje, clientes);

    std::ostringstream log;
    log << "FD " << cliente_fd << " - Nickname: " << nickname;
    registrarEvento("Desconexion", log.str());
}

// Registra un evento en el historial
void Evento::registrarEvento(const std::string& tipoEvento, const std::string& detalle) {
    std::ostringstream evento;
    evento << "[" << tipoEvento << "] " << detalle;
    historialEventos.push_back(evento.str());
}

// Devuelve el historial de eventos
const std::vector<std::string>& Evento::obtenerHistorial() const {
    return historialEventos;
}
