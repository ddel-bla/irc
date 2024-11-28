#include "Cliente.hpp"

/* PARAMETRIZED CONSTRUCTOR */
Cliente::Cliente(int fd): socket_fd(fd), autenticate(false), registred(false) {}

/* DESTRUCTOR */
Cliente::~Cliente(void){
    if (socket_fd >= 0)
        close(socket_fd);
}

/* GETTERS & SETTERS */

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

// Get buffer
const std::string& Cliente::getBuffer() const {
    return buffer;
}

// Is Autenticate
bool Cliente::isAutenticate() const {
    return autenticate;
}

// Is Registred
bool Cliente::isRegistred() const {
    return registred;
}

// Modifica el nickname
void Cliente::setNickname(const std::string& nick) {
    nickname = nick;
}

// Modifica el username
void Cliente::setUsername(const std::string& username) {
    this->username = username;
}

// Set buffer
void Cliente::setBuffer(const std::string& buffer) {
    this->buffer = buffer;
}

// Set Autenticate
void Cliente::setAutenticate(const bool autenticate) {
    this->autenticate = autenticate;
}

// Set Registred
void Cliente::setRegistred(const bool registred) {
    this->registred = registred;
}

void Cliente::clearBuffer(void)
{
    buffer.clear();
}

// Envía un mensaje al cliente
void Cliente::enviarMensaje(const std::string& mensaje) const {
    std::string mensajeFormateado = mensaje + "\r\n"; // Agrega retorno de carro y nueva línea
    ssize_t bytesEnviados = send(socket_fd, mensajeFormateado.c_str(), mensajeFormateado.size(), 0);
    if (bytesEnviados < 0) {
        std::cerr << "Error al enviar mensaje a " << nickname << ": " << strerror(errno) << std::endl;
    }
}
