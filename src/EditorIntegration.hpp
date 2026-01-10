#pragma once
#include <vector>
#include <cstdint>
#include <string>

// Minimal pixel/frame representation for pipeline
struct PixelRGB {
    uint8_t r, g, b;
};
struct FrameData {
    int width = 0;
    int height = 0;
    std::vector<PixelRGB> pixels; // size = width * height
    // Access helper
    inline PixelRGB at(int x, int y) const { return pixels[y * width + x]; }
};

// Abstract editor context (wrap your Geode/LevelEditorLayer pointers here)
struct EditorContext {
    // You should fill these with actual Geode/GD integrations.
    void* levelEditorLayer = nullptr;

    // Grid/world transform
    float startX = 0.f;
    float startY = 0.f;
    float cellSize = 30.f;

    // Group management
    int nextGroupID = 100; // starting group; can read from editor real value

    // Timing
    float songOffsetSeconds = 0.f;
};

namespace EditorIntegration {
    // Places resolution-based blocks per frame and adds color triggers in time.
    bool createObjectsAndTriggers(EditorContext* ctx,
                                  const std::vector<FrameData>& frames,
                                  int fps, int res);

    // Utility to map pixel to editor color id (e.g., 1–999 color channels)
    int mapPixelToColorChannel(const PixelRGB& p);
}