#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
	int socket;
	std::string nickname;
	std::string password;

	// Constructor por defecto
	Client();

	// Constructor que recibe un socket
	Client(int client_socket);

	// Constructor de copia
	Client(const Client& other);

	// Operador de asignación
	Client& operator=(const Client& other);

	// Destructor
	~Client();
};

#endif
