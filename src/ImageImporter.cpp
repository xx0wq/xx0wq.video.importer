// src/VideoImporter.cpp
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/json.hpp>
#include <Geode/utils/file.hpp>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <array>
#include <random>
#include <cmath>
#include <fstream>

using namespace geode::prelude;

// ---------------- Config ----------------
static int GRID_W = 64;
static int GRID_H = 36;
static int PALETTE_K = 12;

// ---------------- Lab conversion ----------------
struct Lab { double L,a,b; };

static Lab rgb2lab(uint8_t r, uint8_t g, uint8_t b) {
    auto f = [](double t) {
        return t > 0.008856 ? pow(t, 1.0/3.0) : (7.787*t + 16.0/116.0);
    };
    auto lin = [](double c) {
        return c <= 0.04045 ? c/12.92 : pow((c+0.055)/1.055, 2.4);
    };
    double R = lin(r/255.0), G = lin(g/255.0), B = lin(b/255.0);
    double X = R*0.4124 + G*0.3576 + B*0.1805;
    double Y = R*0.2126 + G*0.7152 + B*0.0722;
    double Z = R*0.0193 + G*0.1192 + B*0.9505;
    X/=0.95047; Y/=1.00000; Z/=1.08883;
    double fx = f(X), fy = f(Y), fz = f(Z);
    return { 116*fy - 16, 500*(fx-fy), 200*(fy-fz) };
}

static double labDist2(const Lab& a, const Lab& b) {
    double dL=a.L-b.L, da=a.a-b.a, db=a.b-b.b;
    return dL*dL + da*da + db*db;
}

// ---------------- K-means in Lab ----------------
struct Palette {
    std::vector<std::array<uint8_t,3>> rgb; // size K
    std::vector<Lab> lab;                   // size K
};

static Palette kmeansPalette(const std::vector<std::array<uint8_t,3>>& samples, int K, int iters=10) {
    std::vector<Lab> labs; labs.reserve(samples.size());
    for (auto& s : samples) labs.push_back(rgb2lab(s[0],s[1],s[2]));

    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(0, samples.size()-1);
    std::vector<Lab> centers;
    centers.reserve(K);
    for (int k=0;k<K;k++) centers.push_back(labs[dist(rng)]);

    // Track rgb assignments for final centroid RGB
    for (int it=0; it<iters; ++it) {
        std::vector<std::vector<Lab>> groups(K);
        std::vector<std::vector<std::array<uint8_t,3>>> rgbGroups(K);
        groups.reserve(K); rgbGroups.reserve(K);

        for (size_t i=0;i<labs.size();++i) {
            double best=1e12; int idx=0;
            for (int k=0;k<K;k++) {
                double d = labDist2(labs[i], centers[k]);
                if (d<best){best=d; idx=k;}
            }
            groups[idx].push_back(labs[i]);
            rgbGroups[idx].push_back(samples[i]);
        }
        for (int k=0;k<K;k++) {
            if (groups[k].empty()) continue;
            double L=0,a=0,b=0;
            for (auto& v:groups[k]){L+=v.L;a+=v.a;b+=v.b;}
            centers[k].L=L/groups[k].size();
            centers[k].a=a/groups[k].size();
            centers[k].b=b/groups[k].size();
        }
        // On final iteration, compute RGB means
        if (it == iters-1) {
            std::vector<std::array<uint8_t,3>> palRGB;
            palRGB.reserve(K);
            for (int k=0;k<K;k++) {
                if (rgbGroups[k].empty()) { palRGB.push_back({128,128,128}); continue; }
                long R=0,G=0,B=0;
                for (auto& c:rgbGroups[k]){R+=c[0];G+=c[1];B+=c[2];}
                R/=rgbGroups[k].size(); G/=rgbGroups[k].size(); B/=rgbGroups[k].size();
                palRGB.push_back({(uint8_t)R,(uint8_t)G,(uint8_t)B});
            }
            Palette p; p.rgb = palRGB; p.lab = centers; return p;
        }
    }
    Palette p; p.rgb.assign(K, {128,128,128}); p.lab = centers; return p;
}

// ---------------- Nearest palette index ----------------
static int nearestIdx(uint8_t r, uint8_t g, uint8_t b, const std::vector<Lab>& palLab) {
    Lab L = rgb2lab(r,g,b);
    double best=1e12; int idx=0;
    for (int k=0;k<(int)palLab.size();++k) {
        double d=labDist2(L, palLab[k]);
        if (d<best){best=d; idx=k;}
    }
    return idx;
}

// ---------------- FFmpeg decode + downscale to RGB24 ----------------
struct RGBFrame {
    std::vector<uint8_t> data; // size GRID_W*GRID_H*3
};

static std::vector<RGBFrame> decodeFrames(const std::string& path, int targetFPS) {
    std::vector<RGBFrame> frames;
    av_register_all();

    AVFormatContext* fmtCtx = nullptr;
    if (avformat_open_input(&fmtCtx, path.c_str(), nullptr, nullptr) < 0) {
        log::error("Failed to open input {}", path);
        return frames;
    }
    avformat_find_stream_info(fmtCtx, nullptr);

    int vIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (vIdx < 0) {
        log::error("No video stream");
        avformat_close_input(&fmtCtx);
        return frames;
    }
    AVStream* vStream = fmtCtx->streams[vIdx];
    AVCodec* codec = avcodec_find_decoder(vStream->codecpar->codec_id);
    AVCodecContext* cctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(cctx, vStream->codecpar);
    avcodec_open2(cctx, codec, nullptr);

    SwsContext* sws = sws_getContext(
        cctx->width, cctx->height, cctx->pix_fmt,
        GRID_W, GRID_H, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    AVPacket* pkt = av_packet_alloc();
    AVFrame* frm = av_frame_alloc();
    AVFrame* out = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, GRID_W, GRID_H, 1);
    uint8_t* buf = (uint8_t*)av_malloc(numBytes);
    av_image_fill_arrays(out->data, out->linesize, buf, AV_PIX_FMT_RGB24, GRID_W, GRID_H, 1);

    double srcFPS = av_q2d(vStream->r_frame_rate);
    double step = srcFPS / std::max(1, targetFPS);
    double acc = 0.0;

    int64_t lastOut = -1;
    while (av_read_frame(fmtCtx, pkt) >= 0) {
        if (pkt->stream_index != vIdx) { av_packet_unref(pkt); continue; }
        if (avcodec_send_packet(cctx, pkt) < 0) { av_packet_unref(pkt); continue; }
        while (avcodec_receive_frame(cctx, frm) == 0) {
            // Downscale & convert
            sws_scale(sws, frm->data, frm->linesize, 0, cctx->height, out->data, out->linesize);

            // Frame selection for target FPS
            acc += step;
            if (acc >= 1.0) {
                acc -= 1.0;
                RGBFrame rf;
                rf.data.assign(out->data[0], out->data[0] + GRID_W*GRID_H*3);
                frames.push_back(std::move(rf));
            }
        }
        av_packet_unref(pkt);
    }

    av_free(buf);
    av_frame_free(&out);
    av_frame_free(&frm);
    av_packet_free(&pkt);
    sws_freeContext(sws);
    avcodec_close(cctx);
    avcodec_free_context(&cctx);
    avformat_close_input(&fmtCtx);
    return frames;
}

// ---------------- Build JSON from frames ----------------
static nlohmann::json buildJSON(const std::vector<RGBFrame>& frames, int fps, int K) {
    nlohmann::json j;
    j["gridWidth"] = GRID_W;
    j["gridHeight"] = GRID_H;
    j["fps"] = fps;
    j["paletteSize"] = K;
    j["tileIndices"] = nlohmann::json::array();
    j["frames"] = nlohmann::json::array();

    // Sample pixels across frames for global palette
    std::vector<std::array<uint8_t,3>> samples;
    samples.reserve(20000);
    std::mt19937 rng(1337);
    for (size_t fi=0; fi<frames.size(); ++fi) {
        const auto& d = frames[fi].data;
        for (int s=0; s<500; ++s) {
            int idx = std::uniform_int_distribution<int>(0, GRID_W*GRID_H-1)(rng);
            samples.push_back({ d[idx*3+0], d[idx*3+1], d[idx*3+2] });
        }
        if ((int)samples.size() >= 20000) break;
    }
    auto pal = kmeansPalette(samples, K, 10);

    // Fixed tile assignment: choose representative frame (first)
    std::vector<uint8_t> tileIdx; tileIdx.resize(GRID_W*GRID_H);
    if (!frames.empty()) {
        const auto& d = frames[0].data;
        for (int i=0;i<GRID_W*GRID_H;++i) {
            tileIdx[i] = (uint8_t)nearestIdx(d[i*3+0], d[i*3+1], d[i*3+2], pal.lab);
        }
    }
    // Write tileIndices as HxW
    for (int y=0;y<GRID_H;++y) {
        nlohmann::json row = nlohmann::json::array();
        for (int x=0;x<GRID_W;++x) row.push_back(tileIdx[y*GRID_W+x]);
        j["tileIndices"].push_back(row);
    }

    // Per-frame palette colors: average RGB among pixels belonging to each bin
    for (const auto& fr : frames) {
        std::vector<long> R(K,0), G(K,0), B(K,0), C(K,0);
        for (int i=0;i<GRID_W*GRID_H;++i) {
            int k = tileIdx[i];
            R[k] += fr.data[i*3+0];
            G[k] += fr.data[i*3+1];
            B[k] += fr.data[i*3+2];
            C[k] += 1;
        }
        nlohmann::json palFrame = nlohmann::json::array();
        for (int k=0;k<K;++k) {
            uint8_t r = C[k] ? (uint8_t)(R[k]/C[k]) : pal.rgb[k][0];
            uint8_t g = C[k] ? (uint8_t)(G[k]/C[k]) : pal.rgb[k][1];
            uint8_t b = C[k] ? (uint8_t)(B[k]/C[k]) : pal.rgb[k][2];
            palFrame.push_back({ r, g, b });
        }
        j["frames"].push_back(palFrame);
    }
    return j;
}

// ---------------- Runtime video data ----------------
struct VideoData {
    int gridW=0, gridH=0, fps=15, K=12, frameCount=0;
    std::vector<uint8_t> tileIdx;
    std::vector<std::array<uint8_t,3>> frames;
    bool ok=false;
};

static VideoData gData;

static bool loadVideoData() {
    auto path = Mod::get()->getResourcesDir() / "video_data.json";
    auto content = file::readString(path);
    if (!content) { log::warn("video_data.json not found"); return false; }
    auto j = nlohmann::json::parse(content.unwrap(), nullptr, false);
    if (j.is_discarded()) { log::error("Invalid JSON"); return false; }

    gData.gridW = j["gridWidth"];
    gData.gridH = j["gridHeight"];
    gData.fps   = j["fps"];
    gData.K     = j["paletteSize"];
    auto ti = j["tileIndices"];
    for (int y=0;y<gData.gridH;++y)
        for (int x=0;x<gData.gridW;++x)
            gData.tileIdx.push_back((uint8_t)ti[y][x].get<int>());

    auto fr = j["frames"];
    gData.frameCount = fr.size();
    for (int f=0; f<gData.frameCount; ++f) {
        for (int k=0; k<gData.K; ++k) {
            auto rgb = fr[f][k];
            gData.frames.push_back({
                (uint8_t)rgb[0].get<int>(),
                (uint8_t)rgb[1].get<int>(),
                (uint8_t)rgb[2].get<int>()
            });
        }
    }
    gData.ok = true;
    return true;
}

// ---------------- Sprite grid ----------------
class VideoSprite : public CCNode {
public:
    static VideoSprite* create() {
        auto p = new VideoSprite();
        if (p && p->init()) { p->autorelease(); return p; }
        CC_SAFE_DELETE(p); return nullptr;
    }
    bool init() override { return CCNode::init(); }

    void buildGrid() {
        float TILE = 14.f, GAP = 0.f;
        auto win = CCDirector::sharedDirector()->getWinSize();
        float totalW = gData.gridW * (TILE+GAP);
        float totalH = gData.gridH * (TILE+GAP);
        float baseX = (win.width - totalW) * 0.5f;
        float baseY = (win.height - totalH) * 0.5f;

        for (int y=0;y<gData.gridH;++y) {
            for (int x=0;x<gData.gridW;++x) {
                int k = gData.tileIdx[y*gData.gridW+x];
                CCSprite* cell = CCSprite::create("pixel.png");
                if (!cell) {
                    cell = CCSprite::create();
                    auto layer = CCLayerColor::create({255,255,255,255}, TILE, TILE);
                    layer->setAnchorPoint({0.5f,0.5f});
                    cell->addChild(layer);
                    cell->setContentSize({TILE,TILE});
                }
                cell->setPosition({ baseX + x*(TILE+GAP) + TILE*0.5f,
                                    baseY + y*(TILE+GAP) + TILE*0.5f });
                cell->setTag(k);
                cell->setColor({255,255,255});
                addChild(cell);
            }
        }
    }

    void applyPalette(const std::vector<ccColor3B>& pal) {
        auto children = this->getChildren();
        CCARRAY_FOREACH(children, ccObject, obj) {
            auto node = static_cast<CCNode*>(obj);
            int k = node->getTag();
            auto spr = dynamic_cast<CCSprite*>(node);
            if (spr) spr->setColor(pal[k]);
            else node->setColor(pal[k]);
        }
    }
};

// ---------------- Editor hook ----------------
class $modify(VideoEditorLayer, LevelEditorLayer) {
    CCMenu* mMenu = nullptr;
    int mFPS = 15;

    bool init(GJGameLevel* lvl, bool p1) {
        if (!LevelEditorLayer::init(lvl, p1)) return false;

        mMenu = CCMenu::create();
        mMenu->setPosition({0,0});

        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png"),
            this, menu_selector(VideoEditorLayer::onImportVideo)
        );
        btn->setPosition({ 120.f, 60.f });

        // FPS toggle button (cycle 12/15/20)
        auto fpsBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png"),
            this, menu_selector(VideoEditorLayer::onToggleFPS)
        );
        fpsBtn->setPosition({ 180.f, 60.f });

        mMenu->addChild(btn);
        mMenu->addChild(fpsBtn);
        addChild(mMenu, 100);

        return true;
    }

    void onToggleFPS(CCObject*) {
        if (mFPS == 12) mFPS = 15;
        else if (mFPS == 15) mFPS = 20;
        else mFPS = 12;
        log::info("FPS set to {}", mFPS);
    }

    void onImportVideo(CCObject*) {
        file::openFileDialog("Select MP4", {"mp4"}, [this](std::filesystem::path path) {
            if (path.empty()) return;
            auto frames = decodeFrames(path.string(), mFPS);
            if (frames.empty()) { log::error("No frames decoded"); return; }
            auto j = buildJSON(frames, mFPS, PALETTE_K);
            auto outPath = Mod::get()->getResourcesDir() / "video_data.json";
            std::ofstream out(outPath);
            out << j.dump(2);
            out.close();
            log::info("Wrote {}", outPath.string());
        });
    }
};

// ---------------- PlayLayer hook ----------------
class $modify(VideoPlayLayer, PlayLayer) {
    VideoSprite* mSprite = nullptr;
    int mFrame = 0;
    bool mReady = false;

    bool init(GJGameLevel* lvl) {
        if (!PlayLayer::init(lvl)) return false;
        if (!gData.ok) loadVideoData();
        if (!gData.ok) return true;

        mSprite = VideoSprite::create();
        addChild(mSprite, 50);
        mSprite->buildGrid();

        mFrame = 0;
        mReady = true;
        float interval = 1.0f / std::max(1, gData.fps);
        schedule(schedule_selector(VideoPlayLayer::stepVideo), interval);
        applyFrame(mFrame);
        return true;
    }

    void stepVideo(float) {
        if (!mReady) return;
        mFrame = (mFrame + 1) % gData.frameCount;
        applyFrame(mFrame);
    }

    void applyFrame(int f) {
        std::vector<ccColor3B> pal; pal.reserve(gData.K);
        auto eff = this->m_effectManager;
        for (int k=0;k<gData.K;++k) {
            auto rgb = gData.frames[f*gData.K + k];
            ccColor3B c = { rgb[0], rgb[1], rgb[2] };
            pal.push_back(c);
            eff->setColor(k+1, c, false);
        }
        if (mSprite) mSprite->applyPalette(pal);
    }
};