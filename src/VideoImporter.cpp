#include "EditorIntegration.hpp"
#include "VideoDecoder.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>

extern "C" {

// Entry for showing the Import dialog from the editor button/hook.
// Call this from your Geode hook when the user clicks "Video Importer".
__attribute__((visibility("default")))
bool VI_openImportDialog(EditorContext* ctx) {
    if (!ctx) return false;

    // 1) Ask for MP4 path
    std::string path = Utils::openFileDialog("Select MP4 video", {"mp4"});
    if (path.empty()) {
        Utils::notify("No file selected.");
        return false;
    }

    // 2) Ask for FPS and resolution
    int fps = Utils::promptInt("Frames per second (e.g., 24–60)", 30, 1, 120);
    int res = Utils::promptInt("Resolution (blocks per axis, e.g., 32–128)", 64, 8, 256);

    // 3) Decode frames (optionally with FFmpeg)
    VideoDecoder decoder;
    decoder.setTargetResolution(res, res);
    decoder.setTargetFPS(fps);

    Utils::notify("Decoding video…");
    std::vector<FrameData> frames;
    if (!decoder.decode(path, frames)) {
        Utils::notify("Failed to decode video.");
        return false;
    }

    // 4) Create objects + color triggers
    Utils::notify("Placing blocks and creating triggers…");
    bool ok = EditorIntegration::createObjectsAndTriggers(ctx, frames, fps, res);

    Utils::notify(ok ? "Video import complete." : "Import failed during object creation.");
    return ok;
}

}