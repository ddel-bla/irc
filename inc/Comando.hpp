#ifndef COMANDO_HPP
#define COMANDO_HPP

#include <string>
#include <map>
#include "../inc/Evento.hpp"

class Comando {
public:
    static void procesarComando(const std::string& entrada, int cliente_fd,
                                std::map<int, Cliente*>& clientes,
                                std::map<std::string, Canal>& canales,
                                Evento& manejadorEventos);
};

#endif // COMANDO_HPP
