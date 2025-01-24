#include "Logger.hpp"


/* PARAMETRIZED CONSTRUCTOR */
Logger::Logger(const std::string& filename, bool logToFile=false) : logToFile(logToFile)
{
    if (!filename.empty() && logToFile) {
        logFile.open(filename.c_str(), std::ios::out);
        if (!logFile.is_open()) {
            std::cerr << "Error: Can not open logfile: " << filename << "\n";
        }
    }
}

/* DESTRUCTOR */
Logger::~Logger()
{
    if (logFile.is_open()) {
        logFile.close();
    }
}

/* PUBLIC METHODS */
void Logger::log(const std::string& level, const std::string& message)
{
    std::string formattedMessage = formatLog(level, message);
    
    if (logToFile && logFile.is_open())
        logFile << formattedMessage << "\n";
    
    
    std::cout << formattedMessage << "\n";
}

void Logger::info(const std::string& message)
{
    log("INFO", message);
}

void Logger::warning(const std::string& message)
{
    log("WARNING", message);
}

void Logger::error(const std::string& message)
{
    log("ERROR", message);
}

/* PRIVATE METHODS */
std::string Logger::getCurrentTime() const
{
    time_t now = time(0);
    struct tm tstruct;
    char buffer[20];
    tstruct = *localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buffer;
}

std::string Logger::formatLog(const std::string& level, const std::string& message) const
{
    return "[" + getCurrentTime() + "] [" + level + "] " + message;
}

