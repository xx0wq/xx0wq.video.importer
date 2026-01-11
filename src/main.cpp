#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/Loader.hpp>
#include <Geode/Utils.hpp>

#include "VideoImporter.hpp"
#include "EditorInjection.hpp"

using namespace geode;

class VideoImporterMod : public Mod {
public:
    void onLoad() override {
        // Register editor injection
        EditorInjection::registerImportButton();
        log::info("Video Importer Mod loaded successfully!");
    }

    void onUnload() override {
        log::info("Video Importer Mod unloaded.");
    }
};

GEODE_MOD(VideoImporterMod);