#pragma once

// Only include FFmpeg on desktop platforms
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#endif

class VideoImporter {
public:
    static void importVideo(const std::string& path);
};