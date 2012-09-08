#include "demo_page.hpp"
#include "main_menu_page.hpp"
#include <iostream>
#include <Gosu/Gosu.hpp>
#include "page_manager.h"
#include "config.h"

int main(int argc, char* argv[])
{
	Config* config = Config::Instance();
	config->loadArgs(argc, argv);
	config->setFile("config");
	
	PageManager* man = PageManager::Instance();
	man->setCaption(L"Lightshifters SiO2 []");

    if (config->get<bool>("demo", false)) {
        man->load<DemoPage>();
    } else {
        man->load<MainMenuPage>();
    }
	man->show();
	man->Release();
	config->Release();
	return 0;
}