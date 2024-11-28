#include "../inc/Utils.hpp"

std::vector<std::string>    Utils::split(const std::string& str, const std::string& separator)
{
    std::vector<std::string> items;
    size_t start = 0;
    size_t end = str.find(separator);

    while (end != std::string::npos)
    {
        items.push_back(str.substr(start, end - start));
        start = end + separator.length();
        end = str.find(separator, start);
    }

    items.push_back(str.substr(start));

    return (items);
}

std::vector<std::string>    Utils::splitBySpaces(const std::string& str)
{
    std::vector<std::string>    items;
    std::istringstream          stm(str);
    std::string                 token;

    while (stm >> token)
    {
        items.push_back(token);
        token.clear();
    }

    return (items);
}

void    Utils::removeLeadingChar(std::string& str, char c)
{   
    if (!str.empty() && str[0] == c)
        str.erase(str.begin());
}