#include "page_manager.h"
#include "default_page.h"
#include <cassert>
#include "config.h"
#include <Gosu/Bitmap.hpp>
#include <Gosu/Graphics.hpp>

PageManager* PageManager::ms_instance = 0;

PageManager::PageManager()
:Gosu::Window(Config::Instance()->get<unsigned>("screenWidth", 1024),
			Config::Instance()->get<unsigned>("screenHeight", 768)
			, false)
{
}

PageManager::~PageManager()
{
}

PageManager* PageManager::Instance()
{
	if(ms_instance == 0){
		ms_instance = new PageManager();
		ms_instance->load<DefaultPage>();
	}
	return ms_instance;
}

void PageManager::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void PageManager::buttonDown(Gosu::Button bt)
{
	assert(m_pPage);
	m_pPage->buttonDown(bt);
}
void PageManager::buttonUp(Gosu::Button bt)
{
	assert(m_pPage);
	m_pPage->buttonUp(bt);
}
void PageManager::draw()
{
	assert(m_pPage);
	m_pPage->draw();
}
void PageManager::loseFocus()
{
	assert(m_pPage);
	m_pPage->loseFocus();
}
bool PageManager::needsCursor() const
{
	assert(m_pPage);
	return m_pPage->needsCursor();
}
bool PageManager::needsRedraw() const
{
	assert(m_pPage);
	return m_pPage->needsRedraw();
}
void PageManager::releaseMemory()
{
	assert(m_pPage);
	m_pPage->releaseMemory();
}
void PageManager::touchBegan(Gosu::Touch touch)
{
	assert(m_pPage);
	m_pPage->touchBegan(touch);
}
void PageManager::touchEnded(Gosu::Touch touch)
{
	assert(m_pPage);
	m_pPage->touchEnded(touch);
}
void PageManager::touchMoved(Gosu::Touch touch)
{
	assert(m_pPage);
	m_pPage->touchMoved(touch);
}
void PageManager::update()
{
	assert(m_pPage);
	m_pPage->update();
}
#include <GL/glut.h>
#include <Gosu/Utility.hpp>
void PageManager::saveScreenShot(std::string filename)
{
    graphics().flush();
    std::vector<uint8_t> data(graphics().width()*graphics().height()*3, 0);
    glReadPixels(0, 0, graphics().width(), graphics().height(), GL_RGB, GL_BYTE, data.data());
    Gosu::Bitmap bmp(graphics().width(), graphics().height());
    size_t line = graphics().height() -1;
    size_t pos = 0;
    for(size_t i = 0; i < data.size(); i+=3) {
        Gosu::Color col(255, data[i], data[i+1], data[i+2]);
        bmp.setPixel(pos, line, col);
        pos++;
        if (pos == graphics().width()) {
            pos = 0;
            line--;
        }
    }
    Gosu::saveImageFile(bmp, Gosu::widen(filename));
}


std::unique_ptr<Page> PageManager::returnToPrevious()
{
    std::swap(m_pPage, m_pLast);
    if (!m_pPage) {
        m_pPage.reset(new DefaultPage());
    }
    return std::move(m_pLast);
}

std::unique_ptr<Page> PageManager::swap(std::unique_ptr<Page> next)
{
    m_pLast = std::move(m_pPage);
    m_pPage = std::move(next);
    return std::move(m_pLast);
}