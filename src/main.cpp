#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/Loader.hpp>
#include <Geode/Utils.hpp>

#include "VideoImporter.hpp"
#include "EditorInjection.hpp"

using namespace geode;

// Main mod class
class VideoImporterMod : public Mod {
public:
    void onLoad() override {
        log::info("Video Importer Mod v0.70.93 loaded");

        // Register editor injection (adds Import button)
        EditorInjection::registerImportButton();

        // Initialize FFmpeg (optional, but recommended)
        av_register_all();
        avcodec_register_all();
        log::info("FFmpeg initialized");
    }

    void onUnload() override {
        log::info("Video Importer Mod unloaded");
    }
};

// Geode entry point macro
GEODE_MOD(VideoImporterMod);