#pragma once
#include <string>
#include <vector>
#include <map>

namespace gd {
    using string = std::string;
    template <typename T> using vector = std::vector<T>;
    template <typename K, typename V> using map = std::map<K, V>;
}
