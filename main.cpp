#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp> 

using namespace geode::prelude;

class CreditsPopup : public Popup<std::string const&, int, bool> {
protected:
    std::string m_content;
    int m_levelID;
    bool m_isCreator;
    
    // UI Elements
    geode::TextInput* m_input = nullptr;
    CCLabelBMFont* m_label = nullptr;
    CCMenuItemSpriteExtra* m_editBtn = nullptr;
    CCMenuItemSpriteExtra* m_saveBtn = nullptr;

    bool setup(std::string const& credits, int levelID, bool isCreator) override {
        m_content = credits;
        m_levelID = levelID;
        m_isCreator = isCreator;

        this->setTitle("Level Credits");

        auto winSize = m_mainLayer->getContentSize();

        // 1. Create the View Label (Visible by default)
        m_label = CCLabelBMFont::create(m_content.c_str(), "goldFont.fnt");
        m_label->setScale(0.55f);
        m_label->setWidth(280.f);
        m_label->setLineBreakWithoutSpace(true);
        m_label->setAlignment(kCCTextAlignmentCenter);
        m_label->setPosition({winSize.width / 2, winSize.height / 2 + 5.f});
        m_mainLayer->addChild(m_label);

        // 2. Create the Input Field (Hidden by default)
        // Using Geode's TextInput helper
        m_input = geode::TextInput::create(280.f, "Type credits here...");
        m_input->setPosition({winSize.width / 2, winSize.height / 2 + 5.f});
        m_input->setString(m_content);
        m_input->setVisible(false);
        m_mainLayer->addChild(m_input);

        // 3. Edit/Save Buttons (Only if creator)
        if (m_isCreator) {
            auto menu = CCMenu::create();
            menu->setPosition({0, 0});
            m_mainLayer->addChild(menu);

            // Edit Button (Pencil Icon)
            auto editSpr = CCSprite::createWithSpriteFrameName("gj_editBtn_001.png");
            editSpr->setScale(0.8f);
            m_editBtn = CCMenuItemSpriteExtra::create(editSpr, this, menu_selector(CreditsPopup::onEditMode));
            m_editBtn->setPosition({winSize.width - 30.f, 30.f});
            menu->addChild(m_editBtn);

            // Save Button (Green Checkmark) - Hidden initially
            auto saveSpr = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", .8f);
            saveSpr->setScale(0.8f);
            m_saveBtn = CCMenuItemSpriteExtra::create(saveSpr, this, menu_selector(CreditsPopup::onSave));
            m_saveBtn->setPosition({winSize.width / 2, 30.f});
            m_saveBtn->setVisible(false);
            menu->addChild(m_saveBtn);
        }

        return true;
    }

    void onEditMode(CCObject*) {
        // Swap visibility
        m_label->setVisible(false);
        m_editBtn->setVisible(false);
        
        m_input->setVisible(true);
        m_input->focus(); // Automatically focus the keyboard
        m_saveBtn->setVisible(true);
        
        this->setTitle("Edit Credits");
    }

    void onSave(CCObject*) {
        std::string newText = m_input->getString();
        
        // Save to storage
        std::string key = "credits_" + std::to_string(m_levelID);
        Mod::get()->setSavedValue(key, newText);

        // Update local variables and UI
        m_content = newText;
        m_label->setString(newText.c_str());

        // Swap visibility back
        m_input->setVisible(false);
        m_saveBtn->setVisible(false);
        
        m_label->setVisible(true);
        m_editBtn->setVisible(true);
        this->setTitle("Level Credits");
    }

public:
    static CreditsPopup* create(std::string const& credits, int levelID, bool isCreator) {
        auto ret = new CreditsPopup();
        if (ret && ret->initAnchored(320.f, 220.f, credits, levelID, isCreator)) {
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

        // Button logic
        auto label = CCLabelBMFont::create("Credits", "goldFont.fnt");
        label->setScale(0.6f);
        
        auto button = CCMenuItemLabelExtra::create(label, [this](CCObject*) {
            this->onCredits(nullptr);
        });

        // Position: Top Right (safer spot than bottom right usually)
        // Or Bottom Left near the play button.
        // Let's stick to your position but add checks.
        auto win = this->getContentSize();
        button->setPosition({win.width - 45.f, win.height - 30.f}); // Moved to top right to avoid overlap
        menu->addChild(button);

        return true;
    }

    void onCredits(CCObject*) {
        // 1. Get Key
        auto level = m_level;
        std::string key = "credits_" + std::to_string(level->m_levelID);

        // 2. Get Data
        auto credits = Mod::get()->getSavedValue<std::string>(
            key,
            "No credits added yet."
        );

        // 3. Check if user is creator
        // (If AccountID matches, OR if it's a local level in editor mode)
        int accountID = GJAccountManager::sharedState()->m_accountID;
        bool isCreator = (level->m_userID == accountID) || (level->m_levelType == GJLevelType::Editor);

        // 4. Show Popup
        CreditsPopup::create(credits, level->m_levelID, isCreator)->show();
    }
};
