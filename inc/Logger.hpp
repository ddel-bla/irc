#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>

class Logger{

private:
    std::ofstream   logFile;
    bool            toConsole;

    std::string getCurrentTime() const;
    std::string formatLog(const std::string& level, const std::string& message) const;

public:
    /* PARAMETRIZED CONSTRUCTOR */
    Logger(const std::string& filename, bool logToConsole);

    /* DESTRUCTOR */
    ~Logger();

    /* METHODS */
    void    log(const std::string& level, const std::string& message);
    void    info(const std::string& message);
    void    warning(const std::string& message);
    void    error(const std::string& message);

};
#endif // LOGGER_HPP