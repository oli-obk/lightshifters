#include "client_page.h"
#include "server_page.h"
#include "config.h"
#include "page_manager.h"
#include <Gosu/Text.hpp>
#include "main_menu_page.hpp"
#include <Gosu/Math.hpp>

MainMenuPage::MainMenuPage()
:m_Font(PageManager::Instance()->graphics(), Gosu::defaultFontName(), 50)
{
	Config* config = Config::Instance();
    m_selectedEntry = 0;
    m_Entries.push_back(L"Quick Host");
    m_Functions.push_back([config](){
        uint16_t host_port = config->get<uint16_t>("server_port", 50042);
        try {
            PageManager::Instance()->load<ServerPage>(host_port);
        } catch(const std::exception& e) {
            std::cout << "Could not start server: " << e.what() << std::endl;
        }
        });
    m_Entries.push_back(L"Connect to Server");
    m_Functions.push_back([config](){
        try {
            uint16_t host_port = config->get<uint16_t>("client_port", 50043);
            PageManager::Instance()->load<ClientPage>(
                config->get<std::string>("connectTo", "localhost"),
                config->get<uint16_t>("port", 50042),
                host_port);
        } catch(const std::exception& e) {
            std::cout << "Could not start server: " << e.what() << std::endl;
        }
        });
    m_Entries.push_back(L"Exit");
    m_Functions.push_back([](){ PageManager::Instance()->close(); });
}

MainMenuPage::~MainMenuPage()
{
}

void MainMenuPage::draw()
{
    for (size_t i = 0; i < m_Entries.size(); i++) {
        m_Font.draw(m_Entries.at(i), 50, i*50 + 50, 0);
        if (i == m_selectedEntry) {
            m_Font.drawRel(L"*", 40, i*50 + 50, 0, 1, 0);
        }
    }
}

void MainMenuPage::update()
{
}

void MainMenuPage::buttonUp(Gosu::Button b)
{
    if (b == Gosu::kbUp) {
        if (m_selectedEntry > 0) m_selectedEntry--;
    } else if (b == Gosu::kbDown) {
        if (m_selectedEntry < m_Entries.size()-1) m_selectedEntry++;
    } else if (b == Gosu::kbReturn) {
        m_Functions.at(m_selectedEntry)();
    }
}
