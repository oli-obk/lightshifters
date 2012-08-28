#ifndef PAGEMANAGER_HPP
#define PAGEMANAGER_HPP

#include <Gosu/Window.hpp> // Base class: Gosu::Window
#include "page.h"
#include <memory>

class PageManager : public Gosu::Window {

	static PageManager* ms_instance;
	
	std::unique_ptr<Page> m_pPage, m_pLast;

private:
	PageManager(const PageManager& rhs);
	PageManager& operator=(const PageManager& rhs);

public:
	static PageManager* Instance();
	static void Release();

private:
	PageManager();
	~PageManager();

public:
	template<typename T, typename... Args> void load(Args... args);
    template<typename T, typename... Args> std::unique_ptr<Page> swap(Args... args);
	virtual void buttonDown(Gosu::Button);
	virtual void buttonUp(Gosu::Button);
	virtual void draw();
	virtual void loseFocus();
	virtual bool needsCursor() const;
	virtual bool needsRedraw() const;
	virtual void releaseMemory();
	virtual void touchBegan(Gosu::Touch touch);
	virtual void touchEnded(Gosu::Touch touch);
	virtual void touchMoved(Gosu::Touch touch);
	virtual void update();
};

template<typename T, typename... Args> std::unique_ptr<Page> PageManager::swap(Args... args)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T(args...));
	return std::move(m_pLast);
}

template<typename T, typename... Args> void PageManager::load(Args... args)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T(args...));
}

#endif // PAGEMANAGER_HPP
