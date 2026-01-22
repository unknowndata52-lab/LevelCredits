#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class CreditsPopup : public Popup<std::string const&> {
protected:
    bool setup(std::string const& credits) override {
        this->setTitle("Level Credits");

        // "chatFont.fnt" or "bigFont.fnt" are safer defaults if goldFont is missing
        auto label = CCLabelBMFont::create(credits.c_str(), "bigFont.fnt");
        label->setScale(0.5f);
        
        // Handle multi-line text wrapping
        label->setWidth(280.f);
        label->setLineBreakWithoutSpace(true);
        label->setAlignment(kCCTextAlignmentCenter);
        
        // Center the label
        label->setPosition(m_mainLayer->getContentSize() / 2);

        m_mainLayer->addChild(label);
        return true;
    }

public:
    static CreditsPopup* create(std::string const& credits) {
        auto ret = new CreditsPopup();
        if (ret && ret->initAnchored(300.f, 200.f, credits)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(LevelInfoCredits, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) return false;

        auto menu = CCMenu::create();
        // Menu usually needs specific positioning or content size to capture touches correctly
        // 0,0 is fine if the buttons are positioned absolutely relative to the node
        menu->setPosition({0, 0});
        this->addChild(menu);

        // Using a Sprite for the button is often cleaner than a Label, 
        // but a Label works for a simple style.
        auto label = CCLabelBMFont::create("Credits", "bigFont.fnt");
        label->setScale(0.6f);
        
        auto button = CCMenuItemLabelExtra::create(label, [this](CCObject*) {
            this->onCredits(nullptr);
        });

        // Position bottom right
        auto win = this->getContentSize();
        button->setPosition({win.width - 45.f, 45.f});
        
        menu->addChild(button);

        return true;
    }

    void onCredits(CCObject*) {
        // Ensure level exists
        if (!m_level) return;

        std::string key = "credits_" + std::to_string(m_level->m_levelID);

        // Retrieve saved credits
        auto credits = Mod::get()->getSavedValue<std::string>(
            key,
            "No credits have been added for this level."
        );

        CreditsPopup::create(credits)->show();
    }
};
