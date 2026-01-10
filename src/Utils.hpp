#pragma once
#include <string>
#include <vector>

namespace Utils {
    void notify(const std::string& msg);

    // Provide your own file picker integration; for Actions/tests, return a path or empty.
    std::string openFileDialog(const std::string& title,
                               const std::vector<std::string>& exts);

    int promptInt(const std::string& title, int defaultVal,
                  int minVal, int maxVal);
}