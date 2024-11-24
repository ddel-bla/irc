#include "../inc/Canal.hpp"
#include <unistd.h>
#include <sys/socket.h>

void Canal::agregarMiembro(int cliente_fd, Cliente* cliente) {
    miembros[cliente_fd] = cliente;
}

void Canal::eliminarMiembro(int cliente_fd) {
    miembros.erase(cliente_fd);
}

bool Canal::esMiembro(int cliente_fd) const {
    return miembros.find(cliente_fd) != miembros.end();
}

void Canal::enviarMensaje(const std::string& mensaje) const {
    // Evitar advertencia de parámetro no utilizado
    (void)mensaje;

    // Iterar sobre los miembros del canal con un iterador estándar
    for (std::map<int, Cliente*>::const_iterator it = miembros.begin(); it != miembros.end(); ++it) {
        Cliente* cliente = it->second; // Obtiene el puntero al cliente
        if (cliente) {
            cliente->enviarMensaje(mensaje); // Método del cliente para enviar mensajes
        }
    }
}
