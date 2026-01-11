#include "Utils.hpp"
#include <iostream>

void Utils::notify(const std::string& msg) {
    std::cout << "[VideoImporter] " << msg << std::endl;
}

std::string Utils::openFileDialog(const std::string& title,
                                  const std::vector<std::string>& exts) {
    (void)title; (void)exts;
    // In-app: implement platform picker. In CI: return empty (preview uses synthetic frames).
    return std::string();
}

int Utils::promptInt(const std::string& title, int defaultVal,
                     int minVal, int maxVal) {
    (void)title;
    if (defaultVal < minVal) defaultVal = minVal;
    if (defaultVal > maxVal) defaultVal = maxVal;
    return defaultVal;
}

std::string Utils::basename(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}