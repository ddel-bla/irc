#ifndef CANAL_HPP
#define CANAL_HPP

#include <string>
#include <map>
#include "../inc/Cliente.hpp"

class Canal {
public:
    std::string nombre;
    std::string topic;
    std::map<int, Cliente*> miembros;

    Canal(const std::string& nombre) : nombre(nombre), topic("") {}

    void agregarMiembro(int cliente_fd, Cliente* cliente);
    void eliminarMiembro(int cliente_fd);
    bool esMiembro(int cliente_fd) const;
    void enviarMensaje(const std::string& mensaje) const;
};

#endif // CANAL_HPP
