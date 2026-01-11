#include "Export.hpp"
#include "EditorIntegration.hpp"
#include "VideoDecoder.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>

extern "C" {

// Entry point for the importer dialog
VI_EXPORT bool VI_openImportDialog(EditorContext* ctx) {
    if (!ctx) return false;

#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    // Desktop builds: full importer dialog with FFmpeg + cocos
    std::string path = Utils::openFileDialog("Select MP4 video", {"mp4"});
    if (path.empty()) {
        Utils::notify("No file selected.");
        return false;
    }

    int fps = Utils::promptInt("Frames per second", 30, 1, 120);
    int res = Utils::promptInt("Resolution (blocks per axis)", 64, 8, 256);

    VideoDecoder decoder;
    decoder.setTargetResolution(res, res);
    decoder.setTargetFPS(fps);

    std::vector<FrameData> frames;
    if (!decoder.decode(path, frames)) {
        Utils::notify("Failed to decode video.");
        return false;
    }

    bool ok = EditorIntegration::createObjectsAndTriggers(ctx, frames, fps, res);
    Utils::notify(ok ? "Video import complete." : "Import failed during object creation.");
    return ok;
#else
    // Mobile builds: stub implementation
    Utils::notify("Video import is not available on this platform.");
    return false;
#endif
}

} // extern "C"