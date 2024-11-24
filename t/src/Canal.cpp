#include "Canal.hpp"

// Constructor
Canal::Canal(const std::string& nombre) : nombre(nombre), topic("") {}

// Agrega un miembro al canal
void Canal::agregarMiembro(int cliente_fd, Cliente* cliente) {
    if (miembros.find(cliente_fd) == miembros.end()) {
        miembros[cliente_fd] = cliente;
        std::cout << "Cliente con FD " << cliente_fd << " agregado al canal " << nombre << "." << std::endl;
    } else {
        std::cerr << "Cliente con FD " << cliente_fd << " ya es miembro del canal " << nombre << "." << std::endl;
    }
}

// Elimina un miembro del canal
void Canal::eliminarMiembro(int cliente_fd) {
    if (miembros.erase(cliente_fd) > 0) {
        std::cout << "Cliente con FD " << cliente_fd << " eliminado del canal " << nombre << "." << std::endl;
    } else {
        std::cerr << "Cliente con FD " << cliente_fd << " no es miembro del canal " << nombre << "." << std::endl;
    }
}

// Verifica si un cliente es miembro del canal
bool Canal::esMiembro(int cliente_fd) const {
    return miembros.find(cliente_fd) != miembros.end();
}

// Envía un mensaje a todos los miembros del canal
void Canal::enviarMensaje(const std::string& mensaje) const {
    // Evitar advertencia por parámetro no utilizado
    (void)mensaje;

    // Iterar sobre los miembros usando un iterador estándar
    for (std::map<int, Cliente*>::const_iterator it = miembros.begin(); it != miembros.end(); ++it) {
        Cliente* cliente = it->second; // Obtiene el puntero al cliente
        if (cliente) {
            cliente->enviarMensaje(mensaje); // Llama al método enviarMensaje de Cliente
        } else {
            std::cerr << "Error: Cliente nulo en el canal " << nombre << "." << std::endl;
        }
    }
}

// Obtiene la cantidad de miembros en el canal
size_t Canal::cantidadMiembros() const {
    return miembros.size();
}
