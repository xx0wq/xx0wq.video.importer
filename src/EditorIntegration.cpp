#include "EditorIntegration.hpp"
#include "Utils.hpp"

// You’ll replace these stubs with actual Geode/GD calls:
// - createBlock(x, y) -> place object in editor
// - createColorTrigger(groupId, color, atTime) -> schedule color change

// Example object placement stub
static void placeBlock(EditorContext* ctx, int gridX, int gridY, int groupId) {
    // Compute world position
    float x = ctx->startX + gridX * ctx->cellSize;
    float y = ctx->startY + gridY * ctx->cellSize;

    // TODO: Replace with Geode/GD object creation, e.g.:
    // auto obj = editor->createObject(kBlockID, x, y);
    // obj->addToGroup(groupId);

    (void)x; (void)y; (void)groupId;
}

// Example trigger creation stub
static void addColorTrigger(EditorContext* ctx, int groupId, int channel,
                            const PixelRGB& col, float atSeconds) {
    // TODO: Replace with actual color trigger creation:
    // trigger->setGroup(groupId)
    // trigger->setChannel(channel)
    // trigger->setRGB(col.r, col.g, col.b)
    // trigger->setActivateTime(atSeconds + ctx->songOffsetSeconds)

    (void)groupId; (void)channel; (void)col; (void)atSeconds;
}

int EditorIntegration::mapPixelToColorChannel(const PixelRGB& p) {
    // Simple hashing to distribute colors across channels 1–999
    int ch = 1 + ((p.r * 31 + p.g * 17 + p.b * 13) % 999);
    return ch;
}

bool EditorIntegration::createObjectsAndTriggers(EditorContext* ctx,
                                                 const std::vector<FrameData>& frames,
                                                 int fps, int res) {
    if (!ctx || frames.empty()) return false;

    int groupId = ctx->nextGroupID;
    float frameDuration = 1.0f / static_cast<float>(fps);

    // Place static grid of blocks once based on resolution (grouped)
    for (int gy = 0; gy < res; ++gy) {
        for (int gx = 0; gx < res; ++gx) {
            placeBlock(ctx, gx, gy, groupId);
        }
    }

    // For each frame, add color triggers distributed across channels
    for (size_t i = 0; i < frames.size(); ++i) {
        const auto& f = frames[i];
        float t = ctx->songOffsetSeconds + static_cast<float>(i) * frameDuration;

        // Map each cell to a pixel (nearest neighbor downsample)
        for (int gy = 0; gy < res; ++gy) {
            for (int gx = 0; gx < res; ++gx) {
                int px = gx * f.width / res;
                int py = gy * f.height / res;
                PixelRGB p = f.at(px, py);
                int ch = mapPixelToColorChannel(p);
                addColorTrigger(ctx, groupId, ch, p, t);
            }
        }
    }

    // Grouping and finalization (if needed)
    // TODO: Apply editor grouping APIs once integrated.
    return true;
}