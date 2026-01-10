#include "Utils.hpp"
#include <iostream>

void Utils::notify(const std::string& msg) {
    // Replace with Geode/GD UI notifications
    std::cout << "[VideoImporter] " << msg << std::endl;
}

std::string Utils::openFileDialog(const std::string& title,
                                  const std::vector<std::string>& exts) {
    (void)title; (void)exts;
    // For CI builds, return empty to skip actual file picking.
    // In app, use platform picker. For now, return empty and let VideoDecoder synthesize frames.
    return std::string();
}

int Utils::promptInt(const std::string& title, int defaultVal,
                     int minVal, int maxVal) {
    (void)title;
    // In app, display a dialog. For now in CI, use default bounded.
    if (defaultVal < minVal) defaultVal = minVal;
    if (defaultVal > maxVal) defaultVal = maxVal;
    return defaultVal;
}