#pragma once

#include <cstdint>

struct PixelRGB {
    uint8_t r, g, b;
};

struct HSV {
    float h; // Hue: 0..360
    float s; // Saturation: 0..1
    float v; // Value: 0..1
};

namespace ColorMapper {
    HSV rgbToHSV(const PixelRGB& p);
    PixelRGB hsvToRGB(const HSV& hsv);
}