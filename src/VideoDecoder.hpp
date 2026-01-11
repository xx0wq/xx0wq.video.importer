#pragma once
#include <string>
#include <vector>
#include "EditorIntegration.hpp"

class VideoDecoder {
public:
    void setTargetResolution(int w, int h) { m_outW = w; m_outH = h; }
    void setTargetFPS(int fps) { m_outFPS = fps; }

    bool decode(const std::string& mp4Path, std::vector<FrameData>& outFrames);

private:
    int m_outW = 64;
    int m_outH = 64;
    int m_outFPS = 30;
};