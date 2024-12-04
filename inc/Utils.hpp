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
    static std::string  removeLeadingChar(const std::string& str, char c);
    static std::string  getMessageWithoutPrefix(const std::string& message);
    static std::string  getCurrentTimeISO8601();
    static std::string  toUpper(const std::string& input);
     static std::string  intToString(int number);
};

#endif // UTILS_HPP