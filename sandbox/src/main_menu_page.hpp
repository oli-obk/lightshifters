#ifndef MAINMENUPAGE_HPP
#define MAINMENUPAGE_HPP

#include "page.h" // Base class: Page
#include <Gosu/Font.hpp>
#include <functional>

class MainMenuPage : public Page
{
    size_t m_selectedEntry;
    std::vector<std::wstring> m_Entries;
    std::vector<std::function<void()>> m_Functions;
    Gosu::Font m_Font;
public:
    MainMenuPage();
    void update();
    void draw();
    void buttonUp(Gosu::Button);
    ~MainMenuPage();
};

#endif // MAINMENUPAGE_HPP
