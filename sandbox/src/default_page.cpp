#include "default_page.h"
#include <Gosu/Text.hpp>
#include "page_manager.h"

DefaultPage::DefaultPage()
:m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 50)
{
}

DefaultPage::~DefaultPage()
{
}

void DefaultPage::draw()
{
	m_Font.draw(L"This is a dummy", 50, 50, 50);
}
