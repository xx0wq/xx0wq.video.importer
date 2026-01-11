#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/TextInput.hpp>

// FFmpeg headers
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

using namespace geode::prelude;

class VideoImportLayer : public CCLayer {
private:
    CCTextInputNode* fpsInput;
    CCTextInputNode* resInput;

public:
    static VideoImportLayer* create() {
        auto layer = new VideoImportLayer();
        if (layer && layer->init()) {
            layer->autorelease();
            return layer;
        }
        CC_SAFE_DELETE(layer);
        return nullptr;
    }

    bool init() override {
        if (!CCLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto bg = CCLayerColor::create({0, 100, 0, 180});
        this->addChild(bg);

        auto title = CCLabelBMFont::create("Import Video", "bigFont.fnt");
        title->setPosition({winSize.width / 2, winSize.height - 50});
        this->addChild(title);

        fpsInput = CCTextInputNode::create(60, 30, "30", "bigFont.fnt");
        fpsInput->setPosition({200, winSize.height - 120});
        this->addChild(fpsInput);

        resInput = CCTextInputNode::create(120, 30, "640x480", "bigFont.fnt");
        resInput->setPosition({250, winSize.height - 170});
        this->addChild(resInput);

        auto createBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_button_01.png"),
            this,
            menu_selector(VideoImportLayer::onCreate)
        );
        createBtn->setPosition({winSize.width / 2, 80});

        auto menu = CCMenu::create();
        menu->addChild(createBtn);
        menu->setPosition({0, 0});
        this->addChild(menu);

        return true;
    }

    void onCreate(CCObject*) {
        int fps = std::stoi(fpsInput->getString());
        std::string resStr = resInput->getString();
        int width = 0, height = 0;

        size_t xPos = resStr.find('x');
        if (xPos != std::string::npos) {
            width = std::stoi(resStr.substr(0, xPos));
            height = std::stoi(resStr.substr(xPos + 1));
        }

        importVideo("[user-selected-video.mp4]", fps, width, height);
    }

    void importVideo(const std::string& filePath, int fps, int width, int height) {
        av_register_all();
        AVFormatContext* fmtCtx = nullptr;
        if (avformat_open_input(&fmtCtx, filePath.c_str(), nullptr, nullptr) != 0) {
            FLAlertLayer::create("Error", "Could not open video file", "OK")->show();
            return;
        }
        avformat_find_stream_info(fmtCtx, nullptr);

        int videoStream = -1;
        for (unsigned i = 0; i < fmtCtx->nb_streams; i++) {
            if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = i;
                break;
            }
        }
        if (videoStream == -1) {
            FLAlertLayer::create("Error", "No video stream found", "OK")->show();
            return;
        }

        AVCodecParameters* codecPar = fmtCtx->streams[videoStream]->codecpar;
        AVCodec* codec = avcodec_find_decoder(codecPar->codec_id);
        AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecCtx, codecPar);
        avcodec_open2(codecCtx, codec, nullptr);

        AVFrame* frame = av_frame_alloc();
        AVPacket packet;
        SwsContext* swsCtx = sws_getContext(
            codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
            width, height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        auto editor = LevelEditorLayer::get();
        int frameCount = 0;

        while (av_read_frame(fmtCtx, &packet) >= 0) {
            if (packet.stream_index == videoStream) {
                avcodec_send_packet(codecCtx, &packet);
                while (avcodec_receive_frame(codecCtx, frame) == 0) {
                    frameCount++;

                    // Convert frame to RGB
                    uint8_t* rgbData[4];
                    int rgbLinesize[4];
                    av_image_alloc(rgbData, rgbLinesize, width, height, AV_PIX_FMT_RGB24, 1);
                    sws_scale(swsCtx, frame->data, frame->linesize, 0, codecCtx->height, rgbData, rgbLinesize);

                    // Average color
                    long rSum = 0, gSum = 0, bSum = 0;
                    int pixels = width * height;
                    for (int y = 0; y < height; y++) {
                        uint8_t* row = rgbData[0] + y * rgbLinesize[0];
                        for (int x = 0; x < width; x++) {
                            rSum += row[x * 3 + 0];
                            gSum += row[x * 3 + 1];
                            bSum += row[x * 3 + 2];
                        }
                    }
                    int rAvg = rSum / pixels;
                    int gAvg = gSum / pixels;
                    int bAvg = bSum / pixels;

                    // Convert to HSV
                    float h, s, v;
                    {
                        float r = rAvg / 255.0f;
                        float g = gAvg / 255.0f;
                        float b = bAvg / 255.0f;
                        float max = std::max({r, g, b});
                        float min = std::min({r, g, b});
                        v = max;
                        float d = max - min;
                        s = max == 0 ? 0 : d / max;
                        if (max == min) {
                            h = 0;
                        } else if (max == r) {
                            h = fmod(((g - b) / d), 6.0f);
                        } else if (max == g) {
                            h = ((b - r) / d) + 2.0f;
                        } else {
                            h = ((r - g) / d) + 4.0f;
                        }
                        h *= 60.0f;
                        if (h < 0) h += 360.0f;
                    }

                    // Spawn ColorTrigger
                    auto trigger = ColorTrigger::create();
                    trigger->m_hue = h;
                    trigger->m_saturation = s;
                    trigger->m_brightness = v;
                    trigger->setPosition({frameCount * 30.0f / fps, 100.0f});
                    editor->addObject(trigger);

                    av_freep(&rgbData[0]);
                }
            }
            av_packet_unref(&packet);
        }

        av_frame_free(&frame);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        sws_freeContext(swsCtx);

        std::string msg = "Imported " + std::to_string(frameCount) + " frames\n" +
                          "Resolution: " + std::to_string(width) + "x" + std::to_string(height) +
                          "\nFPS: " + std::to_string(fps) +
                          "\nTriggers generated: " + std::to_string(frameCount);
        FLAlertLayer::create("Video Import", msg.c_str(), "OK")->show();
    }
};

class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        auto spr = CCSprite::create("resources/icon.png");
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyEditorUI::onImportPressed)
        );

        auto scaleBtn = m_buttonBar->getChildByID("scale-button");
        if (scaleBtn) {
            btn->setPosition(scaleBtn->getPosition() + ccp(50, 0));
        } else {
            btn->setPosition({100, 50});
        }

        m_buttonBar->addChild(btn);

        return true;
    }

    void onImportPressed(CCObject* sender) {
        auto layer = VideoImportLayer::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
    }
};