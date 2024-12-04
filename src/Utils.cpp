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

std::string    Utils::removeLeadingChar(const std::string& str, char c)
{   
    if (!str.empty() && str[0] == c) {
        return str.substr(1);
    }
    return str;
}

std::string joinIntVector(const std::vector<int>& vec)
{
    std::string         result;
    std::stringstream   ss;
    
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        
        if (i != vec.size() - 1) {
            ss << " ";
        }
    }

    result = ss.str();
    return result;
}

std::string Utils::getMessageWithoutPrefix(const std::string& message)
{
    std::istringstream  ss(message);
    std::string         word;
    std::string         result;

    // SKIP FIRST TWO WORDS
    ss >> word;
    ss >> word;
    
    std::getline(ss, result); 

    // DELETE LEADING ':'
    if (!result.empty() && result[0] == ' ')
        result = result.substr(1);

    return result;
}

std::string Utils::getCurrentTimeISO8601(void)
{
    struct timeval tv;
    char buffer[30];
    std::time_t t;
    struct tm* tm_info;
    std::ostringstream oss;

    gettimeofday(&tv, NULL);

    t = tv.tv_sec;
    tm_info = gmtime(&t);

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", tm_info);

    oss << buffer << "." << (tv.tv_usec / 1000) << "Z";

    return oss.str();
}

std::string Utils::toUpper(const std::string& input)
{
    std::string result = input;
    for (std::string::iterator it = result.begin(); it != result.end(); ++it) {
        *it = std::toupper(*it);
    }
    return result;
}

std::string Utils::intToString(int number)
{
    std::ostringstream oss;
    oss << number;
    return oss.str();
}