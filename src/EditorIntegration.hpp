#pragma once
#include <vector>
#include "ColorMapper.hpp"

struct FrameData {
    int width = 0;
    int height = 0;
    std::vector<PixelRGB> pixels; // size = width * height
    inline PixelRGB at(int x, int y) const { return pixels[y * width + x]; }
};

struct EditorContext {
    void* levelEditorLayer = nullptr; // wire to Geode
    float startX = 0.f;
    float startY = 0.f;
    float cellSize = 30.f;
    int nextGroupID = 100;
    float songOffsetSeconds = 0.f;
};

namespace EditorIntegration {
    // Places blocks once, then uses HVS-based color triggers per frame.
    bool createObjectsAndTriggers(EditorContext* ctx,
                                  const std::vector<FrameData>& frames,
                                  int fps, int res);

    // Preview playback without placing permanent objects (optional stub)
    void previewFrames(const std::vector<FrameData>& frames, int fps);

    // Channel mapping strategy (1..999)
    int mapPixelToColorChannel(const PixelRGB& p);
}