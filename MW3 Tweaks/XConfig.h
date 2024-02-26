#pragma once
#include <string>
#include <vector>
#include <map>

// Returns value of a config string
std::string GetValue(std::string str);

// Converts config into a map
void ConvertToMap(const std::vector<std::string>& config, std::map<std::string, std::string>& map);