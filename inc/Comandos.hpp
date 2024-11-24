#ifndef COMANDOS_HPP
#define COMANDOS_HPP

#include <string>
#include <iostream>

class Comando {
	public:
		void procesar_comando(const std::string& comando, const std::string& argumentos);
};

#endif 
