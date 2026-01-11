#include "VideoDecoder.hpp"
#include "Utils.hpp"

// Stub produces synthetic frames so pipeline builds and runs.
// Replace with FFmpeg/NDK MediaCodec integration for real MP4 decoding.
bool VideoDecoder::decode(const std::string& mp4Path, std::vector<FrameData>& outFrames) {
    if (mp4Path.empty()) {
        Utils::notify("No video path; generating synthetic frames for CI.");
    }

    const int W = m_outW, H = m_outH;
    const int N = std::max(1, m_outFPS * 3); // 3 seconds

    outFrames.clear();
    outFrames.reserve(N);

    for (int i = 0; i < N; ++i) {
        FrameData f;
        f.width = W; f.height = H;
        f.pixels.resize(W * H);

        // Simple animated gradient to validate HVS pipeline
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                uint8_t r = static_cast<uint8_t>((x * 255) / W);
                uint8_t g = static_cast<uint8_t>((y * 255) / H);
                uint8_t b = static_cast<uint8_t>(((i % m_outFPS) * 255) / m_outFPS);
                f.pixels[y * W + x] = { r, g, b };
            }
        }
        outFrames.push_back(std::move(f));
    }

    return true;
}