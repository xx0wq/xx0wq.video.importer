#pragma once
#include <string>
#include <vector>
#include "EditorIntegration.hpp"

struct PanelState {
    int fps = 30;          // 1..120
    int resolution = 64;   // 8..1080
    std::string videoPath;
    std::vector<FrameData> frames;
};

class VideoImportPanel {
public:
    bool init(void* levelEditorLayer);
    void onImportVideo();
    void onPlayPreview();
    void onCreate();

    // UI controls (implement with Geode/ImGui)
    void setFPSSlider(int value);
    void setFPSNumber(int value);
    void setResolutionSlider(int value);
    void setResolutionNumber(int value);

private:
    EditorContext m_ctx;
    PanelState m_state;
    void showPreview();
};