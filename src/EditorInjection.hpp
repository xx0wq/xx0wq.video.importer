#pragma once

// Only include FFmpeg on desktop platforms
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#endif

// Only include cocos-ext on desktop platforms
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
#include <cocos-ext.h>
#endif

class EditorInjection {
public:
    static void registerImportButton();

#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    // These methods only exist on desktop builds
    static void onImportPressed(cocos2d::CCObject* sender);
    static void spawnFrameObject(AVFrame* frame);
#else
    // Mobile builds: stub signatures so code compiles
    static void onImportPressed(void* sender) {}
    static void spawnFrameObject(void* frame) {}
#endif
};