#include "../inc/Servidor.hpp"

#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

Servidor::Servidor(int puerto, const std::string& password)
    : puerto(puerto), password(password), servidor_fd(-1) {}

bool Servidor::iniciar_servidor() {
    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd < 0) {
        std::cerr << "Error al crear el socket: " << strerror(errno) << std::endl;
        return false;
    }

    int opt = 1;
    if (setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error en setsockopt: " << strerror(errno) << std::endl;
        close(servidor_fd);
        return false;
    }

    if (fcntl(servidor_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error al configurar el modo no bloqueante: " << strerror(errno) << std::endl;
        close(servidor_fd);
        return false;
    }

    sockaddr_in direccion;
    std::memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    if (bind(servidor_fd, (struct sockaddr*)&direccion, sizeof(direccion)) < 0) {
        std::cerr << "Error en bind: " << strerror(errno) << std::endl;
        close(servidor_fd);
        return false;
    }

    if (listen(servidor_fd, 5) < 0) {
        std::cerr << "Error en listen: " << strerror(errno) << std::endl;
        close(servidor_fd);
        return false;
    }

    std::cout << "Servidor iniciado en el puerto " << puerto << std::endl;
    return true;
}

void Servidor::ejecutar() {
    struct pollfd pfd;
    pfd.fd = servidor_fd;
    pfd.events = POLLIN;
    fds.push_back(pfd);

    while (true) {
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0) {
            std::cerr << "Error en poll: " << strerror(errno) << std::endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == servidor_fd) {
                    aceptar_cliente();
                } else {
                    procesar_cliente(fds[i].fd);
                }
            }
        }
    }
}

void Servidor::aceptar_cliente() {
    sockaddr_in direccion_cliente;
    socklen_t tam_direccion = sizeof(direccion_cliente);
    int cliente_fd = accept(servidor_fd, (struct sockaddr*)&direccion_cliente, &tam_direccion);

    if (cliente_fd < 0) {
        std::cerr << "Error al aceptar cliente: " << strerror(errno) << std::endl;
        return;
    }

    if (fcntl(cliente_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error al configurar cliente en modo no bloqueante: " << strerror(errno) << std::endl;
        close(cliente_fd);
        return;
    }

    std::string ip_cliente = inet_ntoa(direccion_cliente.sin_addr);
    Cliente* nuevo_cliente = new Cliente(cliente_fd, "Desconocido", "Desconocido", ip_cliente);

    clientes[cliente_fd] = nuevo_cliente;
    manejadorEventos.notificarConexion(cliente_fd, nuevo_cliente->getNickname(), clientes);

    struct pollfd nuevo_pollfd;
    nuevo_pollfd.fd = cliente_fd;
    nuevo_pollfd.events = POLLIN;
    fds.push_back(nuevo_pollfd);
}

void Servidor::procesar_cliente(int cliente_fd) {
    char buffer[512];
    int bytes_leidos = recv(cliente_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_leidos <= 0) {
        if (bytes_leidos == 0)
            std::cout << "Cliente desconectado. FD: " << cliente_fd << std::endl;
        else
            std::cerr << "Error al recibir datos: " << strerror(errno) << std::endl;
        close(cliente_fd);
        eliminar_cliente(cliente_fd);
        return;
    }

    buffer[bytes_leidos] = '\0';
    std::cout << "Mensaje recibido de FD " << cliente_fd << ": " << buffer << std::endl;

    Comando::procesarComando(buffer, cliente_fd, clientes, canales, manejadorEventos);
}

void Servidor::eliminar_cliente(int cliente_fd) {
    if (clientes.find(cliente_fd) != clientes.end()) {
        manejadorEventos.notificarDesconexion(cliente_fd, clientes[cliente_fd]->getNickname(), clientes);
        delete clientes[cliente_fd];
        clientes.erase(cliente_fd);
    }

    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == cliente_fd) {
            fds.erase(fds.begin() + i);
            break;
        }
    }

    close(cliente_fd);
}
