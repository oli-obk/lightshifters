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
    
    void saveScreenShot(std::string filename);

private:
	PageManager();
	~PageManager();

public:
    std::unique_ptr<Page> swap(std::unique_ptr<Page>);
    std::unique_ptr<Page> returnToPrevious();
#ifndef WIN32
	template<typename T, typename... Args> void load(Args... args);
    template<typename T, typename... Args> std::unique_ptr<Page> swap(Args... args);
#else
	template<typename T> void load();
	template<typename T, typename A> void load(A a);
	template<typename T, typename A, typename B> void load(A a, B b);
	template<typename T, typename A, typename B, typename C> void load(A a, B b, C c);
    template<typename T, typename A> std::unique_ptr<Page> swap(A a);
#endif
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

#ifndef WIN32
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
#else
template<typename T, typename A> std::unique_ptr<Page> PageManager::swap(A a)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T(a));
	return std::move(m_pLast);
}

template<typename T> void PageManager::load()
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T());
}
template<typename T, typename A> void PageManager::load(A a)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T(a));
}
template<typename T, typename A, typename B> void PageManager::load(A a, B b)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T(a, b));
}
template<typename T, typename A, typename B, typename C> void PageManager::load(A a, B b, C c)
{
	m_pLast = std::move(m_pPage);
	m_pPage.reset(new T(a, b, c));
}
#endif

#endif // PAGEMANAGER_HPP
