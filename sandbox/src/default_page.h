#ifndef DEFAULT_PAGE_H
#define DEFAULT_PAGE_H

#include "page.h" // Base class: Page
#include <Gosu/Font.hpp>

class DefaultPage : public Page {

private:
	DefaultPage(const DefaultPage& rhs);
	DefaultPage& operator=(const DefaultPage& rhs);
	Gosu::Font m_Font;
public:
	DefaultPage();
	~DefaultPage();
	virtual void draw();
};

#endif // DEFAULT_PAGE_H
