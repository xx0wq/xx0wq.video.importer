#include "VideoDecoder.hpp"
#include "Utils.hpp"

// If you add FFmpeg later, include headers here and link in CMake
// #include <libavformat/avformat.h>
// #include <libswscale/swscale.h>
// #include <libavcodec/avcodec.h>

bool VideoDecoder::decode(const std::string& mp4Path, std::vector<FrameData>& outFrames) {
    if (mp4Path.empty()) return false;

    // If FFmpeg is available, do real decoding.
    // For now, produce a short synthetic test clip so the pipeline works end-to-end.
    const int W = m_outW;
    const int H = m_outH;
    const int N = m_outFPS * 3; // 3 seconds

    outFrames.clear();
    outFrames.reserve(N);

    for (int i = 0; i < N; ++i) {
        FrameData f;
        f.width = W;
        f.height = H;
        f.pixels.resize(W * H);

        // Simple gradient animation to prove timing/trigger density:
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