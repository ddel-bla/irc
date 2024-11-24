#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include "../inc/Evento.hpp"

class Servidor {
private:
    int puerto;
    std::string password;
    int servidor_fd;
    Evento manejadorEventos;
    std::map<int, Cliente*> clientes;           // Mapa de clientes
    std::map<std::string, Canal> canales;       // Mapa de canales
    std::vector<pollfd> fds;                    // Vector de descriptores para `poll`

public:
    Servidor(int puerto, const std::string& password);
    bool iniciar_servidor();
    void ejecutar();
    void aceptar_cliente();
    void procesar_cliente(int cliente_fd);
    void eliminar_cliente(int cliente_fd);
};

#endif // SERVIDOR_HPP
