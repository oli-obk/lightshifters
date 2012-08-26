#include "page_manager.h"
#include "default_page.h"
#include <cassert>
#include "config.h"

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
