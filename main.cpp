#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class CreditsPopup : public Popup<std::string const&, int, bool> {
protected:
    std::string m_currentCredits;
    int m_levelID;
    bool m_canEdit;

    CCLabelBMFont* m_label = nullptr;
    geode::TextInput* m_input = nullptr;
    CCMenuItemSpriteExtra* m_editBtn = nullptr;
    CCMenuItemSpriteExtra* m_saveBtn = nullptr;

    bool setup(std::string const& credits, int levelID, bool canEdit) override {
        m_currentCredits = credits;
        m_levelID = levelID;
        m_canEdit = canEdit;

        this->setTitle("Level Credits");

        auto winSize = m_mainLayer->getContentSize();

        // Display Label
        m_label = CCLabelBMFont::create(m_currentCredits.c_str(), "goldFont.fnt");
        m_label->setScale(0.5f);
        m_label->setWidth(250.f);
        m_label->setAlignment(kCCTextAlignmentCenter);
        m_label->setPosition(winSize / 2);
        m_mainLayer->addChild(m_label);

        // Edit Input (Hidden by default)
        m_input = geode::TextInput::create(250.f, "Enter credits...");
        m_input->setPosition(winSize / 2);
        m_input->setString(m_currentCredits);
        m_input->setVisible(false);
        m_mainLayer->addChild(m_input);

        if (m_canEdit) {
            auto menu = CCMenu::create();
            menu->setPosition({0, 0});
            m_mainLayer->addChild(menu);

            // Edit Button (Pencil)
            auto editSpr = CCSprite::createWithSpriteFrameName("gj_editBtn_001.png");
            editSpr->setScale(0.8f);
            m_editBtn = CCMenuItemSpriteExtra::create(editSpr, this, menu_selector(CreditsPopup::onToggleEdit));
            m_editBtn->setPosition({winSize.width - 30.f, 30.f});
            menu->addChild(m_editBtn);

            // Save Button
            auto saveSpr = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", 0.8f);
            m_saveBtn = CCMenuItemSpriteExtra::create(saveSpr, this, menu_selector(CreditsPopup::onSave));
            m_saveBtn->setPosition({winSize.width / 2, 35.f});
            m_saveBtn->setVisible(false);
            menu->addChild(m_saveBtn);
        }

        return true;
    }

    void onToggleEdit(CCObject*) {
        m_label->setVisible(false);
        m_editBtn->setVisible(false);

        m_input->setVisible(true);
        m_saveBtn->setVisible(true);
        this->setTitle("Editing Credits");
    }

    void onSave(CCObject*) {
        m_currentCredits = m_input->getString();
        
        // Save to Geode storage
        std::string key = "credits_" + std::to_string(m_levelID);
        Mod::get()->setSavedValue(key, m_currentCredits);

        // UI Feedback
        m_label->setString(m_currentCredits.c_str());
        m_label->setVisible(true);
        m_editBtn->setVisible(true);

        m_input->setVisible(false);
        m_saveBtn->setVisible(false);
        this->setTitle("Level Credits");
    }

public:
    static CreditsPopup* create(std::string const& credits, int levelID, bool canEdit) {
        auto ret = new CreditsPopup();
        if (ret && ret->initAnchored(300.f, 200.f, credits, levelID, canEdit)) {
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

        auto label = CCLabelBMFont::create("Credits", "goldFont.fnt");
        label->setScale(0.5f);
        auto button = CCMenuItemLabelExtra::create(label, [this](CCObject*) {
            this->onCredits(nullptr);
        });

        auto win = this->getContentSize();
        button->setPosition({win.width - 40.f, 70.f});
        menu->addChild(button);

        return true;
    }

    void onCredits(CCObject*) {
        if (!m_level) return;

        std::string key = "credits_" + std::to_string(m_level->m_levelID);
        auto credits = Mod::get()->getSavedValue<std::string>(key, "No credits added.");

        // Check if user is the creator (userID 0 check for local levels)
        bool isCreator = (m_level->m_userID == GJAccountManager::sharedState()->m_accountID) || (m_level->m_levelType == GJLevelType::Editor);

        CreditsPopup::create(credits, m_level->m_levelID, isCreator)->show();
    }
};
