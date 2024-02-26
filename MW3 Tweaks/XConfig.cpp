#include "XConfig.h"

std::string GetValue(std::string str)
{
    size_t pos = str.find_last_of("=");
    return str.substr(++pos, str.length() - pos);
}

void ConvertToMap(const std::vector<std::string>& config, std::map<std::string, std::string>& map)
{
    // Loop over each string in the vector
    for (int i = 0; i < config.size(); i++)
    {
        std::string key, value;
        // Find the position of '=' delimeter
        const size_t delim = config.at(i).find("=");

        // Get the key and value
        key = config.at(i).substr(0, delim);
        value = config.at(i).substr(delim + 1, config.at(i).size());

        // Add key value pair to map
        map[key] = value;
    }
}
