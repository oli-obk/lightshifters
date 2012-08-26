#include <iostream>
#include <Gosu/Gosu.hpp>
#include "page_manager.h"
#include "config.h"
#include "space_page.h"

int main(int argc, char* argv[])
{
	Config* config = Config::Instance();
	config->loadArgs(argc, argv);
	config->setFile("config");
	
	PageManager* man = PageManager::Instance();
	man->setCaption(L"Lightshifters SiO2 []");
	man->load<SpacePage>();
	man->show();
	man->Release();
	config->Release();
	return 0;
}