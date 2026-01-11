#pragma once
#include <string>
extern "C" {
#include <libavcodec/avcodec.h>
}

class EditorInjection {
public:
    static void registerImportButton();
    static void onImportPressed(cocos2d::CCObject* sender);
    static void spawnFrameObject(AVFrame* frame);
};