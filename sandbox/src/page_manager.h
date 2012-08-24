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
	template<typename T> std::unique_ptr<Page> load(bool return_old_page = false);
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


template<typename T> std::unique_ptr<Page> PageManager::load(bool return_old_page)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T());
	if (return_old_page) return std::move(m_pLast);
	return std::move(std::unique_ptr<T>());
}

#endif // PAGEMANAGER_HPP
