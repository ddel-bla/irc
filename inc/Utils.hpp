#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class Utils
{
private:
    Utils() {}
public:
    static std::vector<std::string> split(const std::string& str, const std::string& separator);
    static std::vector<std::string> splitBySpaces(const std::string& str);
    static void removeLeadingChar(std::string& str, char c);

};  // UTILS.HPP