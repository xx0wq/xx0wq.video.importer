#include "ColorMapper.hpp"
#include <algorithm>
#include <cmath>

HSV ColorMapper::rgbToHSV(const PixelRGB& p) {
    float r = p.r / 255.f, g = p.g / 255.f, b = p.b / 255.f;
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;

    HSV out;
    if (delta == 0.f) out.h = 0.f;
    else if (max == r) out.h = 60.f * std::fmod(((g - b) / delta), 6.f);
    else if (max == g) out.h = 60.f * (((b - r) / delta) + 2.f);
    else               out.h = 60.f * (((r - g) / delta) + 4.f);
    if (out.h < 0.f) out.h += 360.f;

    out.s = (max == 0.f) ? 0.f : (delta / max);
    out.v = max;
    return out;
}

PixelRGB ColorMapper::hsvToRGB(const HSV& hsv) {
    float c = hsv.v * hsv.s;
    float x = c * (1.f - std::fabs(std::fmod(hsv.h / 60.f, 2.f) - 1.f));
    float m = hsv.v - c;

    float r, g, b;
    if      (hsv.h < 60.f)  { r = c; g = x; b = 0.f; }
    else if (hsv.h < 120.f) { r = x; g = c; b = 0.f; }
    else if (hsv.h < 180.f) { r = 0.f; g = c; b = x; }
    else if (hsv.h < 240.f) { r = 0.f; g = x; b = c; }
    else if (hsv.h < 300.f) { r = x; g = 0.f; b = c; }
    else                    { r = c; g = 0.f; b = x; }

    PixelRGB out {
        static_cast<uint8_t>((r + m) * 255.f),
        static_cast<uint8_t>((g + m) * 255.f),
        static_cast<uint8_t>((b + m) * 255.f)
    };
    return out;
}