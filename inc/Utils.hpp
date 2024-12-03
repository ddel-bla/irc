#ifndef UTILS_HPP
# define UTILS_HPP 

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <sys/time.h>

class Utils
{
private:
    Utils() {}
public:
    static std::vector<std::string> split(const std::string& str, const std::string& separator);
    static std::vector<std::string> splitBySpaces(const std::string& str);
    static void removeLeadingChar(std::string& str, char c);
    static std::string  getMessageWithoutPrefix(const std::string& message);
    static std::string  getCurrentTimeISO8601();
};

#endif // UTILS_HPP