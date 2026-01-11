#include "VideoImportPanel.hpp"
#include "VideoDecoder.hpp"
#include "Utils.hpp"

bool VideoImportPanel::init(void* levelEditorLayer) {
    m_ctx.levelEditorLayer = levelEditorLayer;
    m_ctx.startX = 0.f; m_ctx.startY = 0.f; m_ctx.cellSize = 30.f;
    m_ctx.nextGroupID = 100; m_ctx.songOffsetSeconds = 0.f;

    // Build UI: sliders + numeric inputs, import button, preview area, blue play button, create button.
    Utils::notify("Video Import Panel initialized.");
    return true;
}

void VideoImportPanel::setFPSSlider(int value) {
    if (value < 1) value = 1;
    if (value > 120) value = 120;
    m_state.fps = value;
}
void VideoImportPanel::setFPSNumber(int value) { setFPSSlider(value); }

void VideoImportPanel::setResolutionSlider(int value) {
    if (value < 8) value = 8;
    if (value > 1080) value = 1080;
    m_state.resolution = value;
}
void VideoImportPanel::setResolutionNumber(int value) { setResolutionSlider(value); }

void VideoImportPanel::onImportVideo() {
    m_state.videoPath = Utils::openFileDialog("Select MP4", {"mp4"});
    if (m_state.videoPath.empty()) {
        Utils::notify("No file selected.");
        return;
    }

    VideoDecoder decoder;
    decoder.setTargetResolution(m_state.resolution, m_state.resolution);
    decoder.setTargetFPS(m_state.fps);

    if (!decoder.decode(m_state.videoPath, m_state.frames)) {
        Utils::notify("Failed to decode video.");
        return;
    }

    showPreview();
}

void VideoImportPanel::showPreview() {
    std::string name = Utils::basename(m_state.videoPath);
    Utils::notify(std::string("Preview ready: ") + name);
    // Show preview tab with filename and blue ▶ Play button
}

void VideoImportPanel::onPlayPreview() {
    EditorIntegration::previewFrames(m_state.frames, m_state.fps);
}

void VideoImportPanel::onCreate() {
    bool ok = EditorIntegration::createObjectsAndTriggers(&m_ctx, m_state.frames,
                                                          m_state.fps, m_state.resolution);
    Utils::notify(ok ? "Create complete." : "Create failed.");
}