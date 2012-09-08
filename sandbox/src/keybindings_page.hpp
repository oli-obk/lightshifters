#ifndef KEYBINDINGSPAGE_HPP
#define KEYBINDINGSPAGE_HPP

#include "page.h" // Base class: Page
#include <Gosu/Font.hpp>

class KeybindingsPage : public Page
{    
    enum Key
    {
        kBeginList,
        kForward = kBeginList, kBackward, kTurbo, kLeft, kRight, kUp, kDown, kSpinLeft, kSpinRight, kEndList
    };
    Key CurrentState;
    Gosu::Font font;
public:
    KeybindingsPage();
    ~KeybindingsPage();
    void update();
    void draw();
    void buttonUp(Gosu::Button);
};

#endif // KEYBINDINGSPAGE_HPP
