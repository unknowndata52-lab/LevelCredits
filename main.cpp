#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/Custom/UIButton.hpp>

using namespace geode::prelude;

class CreditsPopup : public Popup<std::string const&> {
protected:
    bool setup(std::string const& credits) override {
        this->setTitle("Level Credits");

        auto label = CCLabelBMFont::create(credits.c_str(), "goldFont.fnt");
        label->setScale(0.5f);
        label->setWidth(280.f);
        label->setAlignment(kCCTextAlignmentLeft);
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
        menu->setPosition({0, 0});
        this->addChild(menu);

        // Use label as button
        auto label = CCLabelBMFont::create("Credits", "goldFont.fnt");
        label->setScale(0.6f);
        auto button = CCMenuItemLabelExtra::create(label, [this](CCObject*) {
            this->onCredits(nullptr);
        });

        auto win = this->getContentSize();
        button->setPosition({win.width - 45.f, 45.f});
        menu->addChild(button);

        return true;
    }

    void onCredits(CCObject*) {
        auto level = m_level;
        std::string key = "credits_" + std::to_string(level->levelID());

        auto credits = Mod::get()->getSavedValue<std::string>(
            key,
            "No credits have been added for this level."
        );

        CreditsPopup::create(credits)->show();
    }
};

$on_mod(Loaded) {
    log::info("Level Credits mod loaded successfully");
}
