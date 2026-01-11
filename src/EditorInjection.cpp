#include "EditorInjection.hpp"
#include "VideoImporter.hpp"
#include <Geode/Bindings.hpp>
#include <Geode/Utils.hpp>

using namespace geode;

void EditorInjection::registerImportButton() {
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
}

void EditorInjection::onImportPressed(CCObject* sender) {
    std::string videoPath = "Resources/test.mp4"; // replace with file picker later
    VideoImporter::importVideo(videoPath);
}

void EditorInjection::spawnFrameObject(AVFrame* frame) {
    auto editLayer = EditLevelLayer::get();
    if (!editLayer) return;

    // Simplified: spawn a placeholder object per frame
    auto obj = GameObject::createWithKey(1); // basic block object
    obj->setPosition({200, 200}); // could map frame pixels to positions
    editLayer->m_objectLayer->addChild(obj);

    geode::log::info("Spawned object for video frame.");
}