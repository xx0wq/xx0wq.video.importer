#pragma once
#include <string>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class VideoImporter {
public:
    static void importVideo(const std::string& path);
};