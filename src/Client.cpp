#include "../inc/Client.hpp"

// Constructor por defecto
Client::Client() 
	: socket(-1), nickname("Anonymous"), password("Unknown") {}

// Constructor que recibe un socket
Client::Client(int client_socket)
	: socket(client_socket), nickname("Anonymous"), password("Unknown") {}

// Constructor de copia
Client::Client(const Client& other) {
	socket = other.socket;
	nickname = other.nickname;
	password = other.password;
}

// Operador de asignación
Client& Client::operator=(const Client& other) {
	if (this != &other) {
		socket = other.socket;
		nickname = other.nickname;
		password = other.password;
	}
	return *this;
}

// Destructor
Client::~Client() {
	// Liberar
}
