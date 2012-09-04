#include <Text.hpp>
#include "page_manager.h"
#include "demo_page.hpp"
#include <random>
#include <cassert>
#include <sstream>
#include <Gosu/Graphics.hpp>

DemoPage::DemoPage()
:m_Quaternion(Quaternion::identity())
,m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 20)
{
    std::uniform_real_distribution<double> posgen(-1000, 1000);
    std::uniform_int_distribution<uint32_t> tempgen(0, 60000);
    std::mt19937 engine; // Mersenne twister MT19937
    for (int i = 0; i < 100; i++) {
        Vector pos(posgen(engine), posgen(engine), posgen(engine));
        uint32_t temp = tempgen(engine);
        m_Positions.push_back(pos);
        m_Colors.push_back(Temperature(temp).color());
    }
}

DemoPage::~DemoPage()
{
}

#include "Matrix.h"

enum class DemoStep : uint8_t
{
    rotate_right, rotate_down, rotate_around, end_demo
};

static DemoStep g_DemoStep = DemoStep::rotate_right;
static size_t g_uTimer = 0;
static size_t g_Index = 0;

void DemoPage::draw()
{
    Renderable rend = Renderable::temporary();
    Matrix mat = m_Quaternion.inverted().toMatrix();
    double wdt = PageManager::Instance()->graphics().width();
    double hgt = PageManager::Instance()->graphics().height();
    for(size_t i = 0; i < m_Positions.size(); i++) {
        rend.setImageName(L"sphere.png");
        rend.setScale(0.1);
        rend.setColor(m_Colors[i]);
        rend.setPosition(m_Positions[i]);
        rend.draw(mat, wdt, hgt);
    }
    switch(g_DemoStep) {
        case DemoStep::rotate_right:
            m_Font.draw(L"Rotating to the right", 0, 0, 10);
            break;
        case DemoStep::rotate_down:
            m_Font.draw(L"Rotating downwards", 0, 0, 10);
            break;
        case DemoStep::rotate_around:
            m_Font.draw(L"Spinning to the right", 0, 0, 10);
            break;
    }
    g_Index++;
    if ((g_Index%5)==0) {
        std::stringstream ss;
        ss << "demo";
        if (g_Index < 1000) ss << "0";
        if (g_Index < 100) ss << "0";
        if (g_Index < 10) ss << "0";
        ss << g_Index << ".png";
        PageManager::Instance()->saveScreenShot(ss.str());
    }
}

#include <Gosu/Math.hpp>

void DemoPage::update()
{
	double angle = Gosu::degreesToRadians(1);
    switch(g_DemoStep) {
        case DemoStep::rotate_right:
            m_Quaternion *= Quaternion::fromAxisAngle(Vector::UP, angle);
            break;
        
        case DemoStep::rotate_down:
            m_Quaternion *= Quaternion::fromAxisAngle(Vector::RIGHT, -angle);
            break;
            
        case DemoStep::rotate_around:
            m_Quaternion *= Quaternion::fromAxisAngle(Vector::FORWARD, -angle);
            break;
        default:
            PageManager::Instance()->close();
            break;
    }
    g_uTimer++;
    if (g_uTimer == 500) {
         (*(uint8_t*)(&g_DemoStep))++;
         g_uTimer = 0;
    }
}

