#include "EditorInjection.hpp"
#include "VideoImporter.hpp"
#include <Geode/Bindings.hpp>
#include <Geode/Utils.hpp>

using namespace geode;

void EditorInjection::registerImportButton() {
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    auto editLayer = EditLevelLayer::get();
    if (!editLayer) return;

    auto menu = editLayer->m_editTabMenu;
    if (!menu) return;

    auto importBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_importBtn_001.png"),
        editLayer,
        menu_selector(EditorInjection::onImportPressed)
    );
    importBtn->setPosition({100, 50}); // adjust position near Scale button
    menu->addChild(importBtn);

    geode::log::info("Import button injected into editor.");
#else
    geode::log::info("Import button not available on this platform.");
#endif
}

void EditorInjection::onImportPressed(
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    CCObject* sender
#else
    void* sender
#endif
) {
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    std::string videoPath = "Resources/test.mp4"; // replace with file picker later
    VideoImporter::importVideo(videoPath);
#else
    (void)sender;
    geode::log::info("Video import not supported on this platform.");
#endif
}

void EditorInjection::spawnFrameObject(
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    AVFrame* frame
#else
    void* frame
#endif
) {
#if !defined(GEODE_PLATFORM_ANDROID) && !defined(GEODE_PLATFORM_IOS)
    auto editLayer = EditLevelLayer::get();
    if (!editLayer) return;

    auto obj = GameObject::createWithKey(1); // basic block object
    obj->setPosition({200, 200});
    editLayer->m_objectLayer->addChild(obj);

    geode::log::info("Spawned object for video frame.");
#else
    (void)frame;
    geode::log::info("Frame spawning not supported on this platform.");
#endif
}