#include "EditorIntegration.hpp"
#include "Utils.hpp"
#include "ColorMapper.hpp"

// Replace these with Geode/GD implementations:
static void placeBlock(EditorContext* ctx, int gridX, int gridY, int groupId) {
    (void)ctx; (void)gridX; (void)gridY; (void)groupId;
    // Compute world position and create the block in the editor, assign to groupId.
}

static void addColorTrigger(EditorContext* ctx, int groupId, int channel,
                            const PixelRGB& rgb, float atSeconds) {
    (void)ctx; (void)groupId; (void)channel; (void)rgb; (void)atSeconds;
    // Create a color trigger targeting 'channel', scheduled at 'atSeconds',
    // and apply the RGB values. Associate trigger with 'groupId' if needed.
}

int EditorIntegration::mapPixelToColorChannel(const PixelRGB& p) {
    // Distribute across channels for concurrency; tune as needed
    return 1 + ((p.r * 31 + p.g * 17 + p.b * 13) % 999);
}

bool EditorIntegration::createObjectsAndTriggers(EditorContext* ctx,
                                                 const std::vector<FrameData>& frames,
                                                 int fps, int res) {
    if (!ctx || frames.empty()) return false;

    int groupId = ctx->nextGroupID;
    float frameDuration = 1.0f / static_cast<float>(fps);

    // Place static resolution grid once
    for (int gy = 0; gy < res; ++gy)
        for (int gx = 0; gx < res; ++gx)
            placeBlock(ctx, gx, gy, groupId);

    // Per-frame HVS color triggers
    for (size_t i = 0; i < frames.size(); ++i) {
        const auto& f = frames[i];
        float t = ctx->songOffsetSeconds + static_cast<float>(i) * frameDuration;

        for (int gy = 0; gy < res; ++gy) {
            for (int gx = 0; gx < res; ++gx) {
                int px = gx * f.width  / res;
                int py = gy * f.height / res;
                PixelRGB src = f.at(px, py);

                // Convert RGB -> HSV then back to RGB to enforce HVS mapping behavior
                HSV hsv = ColorMapper::rgbToHSV(src);
                PixelRGB mapped = ColorMapper::hsvToRGB(hsv);

                int channel = mapPixelToColorChannel(src);
                addColorTrigger(ctx, groupId, channel, mapped, t);
            }
        }
    }

    return true;
}

void EditorIntegration::previewFrames(const std::vector<FrameData>& frames, int fps) {
    (void)frames; (void)fps;
    // Implement temporary preview using scheduled triggers,
    // or draw to a preview layer without placing permanent objects.
    Utils::notify("Preview started (stub).");
}