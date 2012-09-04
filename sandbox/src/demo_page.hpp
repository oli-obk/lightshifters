#ifndef DEMOPAGE_HPP
#define DEMOPAGE_HPP

#include "renderable.h"
#include "page.h" // Base class: Page
#include <vector>
#include "Vector.h"
#include <Gosu/Color.hpp>
#include "Quaternion.h"
#include <Gosu/Font.hpp>

class DemoPage : public Page
{
private:
    DemoPage(const DemoPage& rhs);
    DemoPage& operator=(const DemoPage& rhs);

public:
    void update();
    void draw();
    std::vector<Vector> m_Positions;
    std::vector<Gosu::Color> m_Colors;
    Quaternion m_Quaternion;
    Gosu::Font m_Font;
    DemoPage();
    ~DemoPage();

};

#endif // DEMOPAGE_HPP
