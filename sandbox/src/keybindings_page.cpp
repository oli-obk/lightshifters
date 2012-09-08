#include "page_manager.h"
#include "config.h"
#include <Gosu/Text.hpp>
#include "keybindings_page.hpp"

KeybindingsPage::KeybindingsPage()
:font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 20)
{
    CurrentState = kBeginList;
}

KeybindingsPage::~KeybindingsPage()
{
}

void KeybindingsPage::draw()
{
    std::wstringstream text;
    text << "Press the key to bind to ";
    switch( CurrentState )
    {
        case kForward:
            text << "move forward";
        break;
        case kBackward:
            text << "move backward";
        break;
        case kUp:
            text << "move upward";
        break;
        case kDown:
            text << "move downward";
        break;
        case kRight:
            text << "move right";
        break;
        case kLeft:
            text << "move left";
        break;
        case kTurbo:
            text << "activate turbo";
        break;
        case kSpinLeft:
            text << "spin left";
        break;
        case kSpinRight:
            text << "spin right";
        break;
        default:
            text << " THIS IS A BUG";
        break;
    }
    font.draw(text.str(), 20, 20, 20);
}

void KeybindingsPage::update()
{
    if (CurrentState == kEndList) {
        PageManager::Instance()->returnToPrevious();
        return;
    }
}

void KeybindingsPage::buttonUp(Gosu::Button b)
{
    Config * cfg = Config::Instance();
    switch(CurrentState)
    {
        case kForward:
            cfg->set("keyForward", b);
        break;
        case kBackward:
            cfg->set("keyBackward", b);
        break;
        case kUp:
            cfg->set("keyStrafeUp", b);
        break;
        case kDown:
            cfg->set("keyStrafeDown", b);
        break;
        case kRight:
            cfg->set("keyStrafeRight", b);
        break;
        case kLeft:
            cfg->set("keyStrafeLeft", b);
        break;
        case kSpinLeft:
            cfg->set("keySpinLeft", b);
        break;
        case kSpinRight:
            cfg->set("keySpinRight", b);
        break;
        case kTurbo:
            cfg->set("keyTurbo", b);
        break;
        default:
            std::cerr << "this is a bug" << std::endl;
        break;
    }
    CurrentState = static_cast<Key>(static_cast<int>(CurrentState) + 1);
}
