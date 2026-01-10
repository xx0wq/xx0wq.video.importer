#pragma once
#include "EditorIntegration.hpp"
#include <string>
#include <vector>

class VideoDecoder {
public:
    void setTargetResolution(int w, int h) { m_outW = w; m_outH = h; }
    void setTargetFPS(int fps) { m_outFPS = fps; }

    // Decode frames into FrameData vector.
    // Returns true on success.
    bool decode(const std::string& mp4Path, std::vector<FrameData>& outFrames);

private:
    int m_outW = 64;
    int m_outH = 64;
    int m_outFPS = 30;

    // Optional: FFmpeg initialization flags
    bool m_useFFmpeg = true;
};