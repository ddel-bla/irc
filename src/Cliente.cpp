#include "Cliente.hpp"
#include <cstring> // Para strerror

// Constructor
Cliente::Cliente(int fd, const std::string& nick, const std::string& user, const std::string& host)
    : socket_fd(fd), nickname(nick), username(user), hostname(host) {}

// Destructor
Cliente::~Cliente() {
    // Cierra el socket al destruir el objeto
    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

// Retorna el descriptor del socket
int Cliente::getSocketFD() const {
    return socket_fd;
}

// Retorna el nickname
const std::string& Cliente::getNickname() const {
    return nickname;
}

// Retorna el username
const std::string& Cliente::getUsername() const {
    return username;
}

// Retorna el hostname
const std::string& Cliente::getHostname() const {
    return hostname;
}

// Modifica el nickname
void Cliente::setNickname(const std::string& nick) {
    nickname = nick;
}

// Envía un mensaje al cliente
void Cliente::enviarMensaje(const std::string& mensaje) const {
    std::string mensajeFormateado = mensaje + "\r\n"; // Agrega retorno de carro y nueva línea
    ssize_t bytesEnviados = send(socket_fd, mensajeFormateado.c_str(), mensajeFormateado.size(), 0);
    if (bytesEnviados < 0) {
        std::cerr << "Error al enviar mensaje a " << nickname << ": " << strerror(errno) << std::endl;
    }
}
