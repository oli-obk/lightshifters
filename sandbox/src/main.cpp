#include <iostream>
#include <Gosu/Gosu.hpp>
#include "page_manager.h"
#include "config.h"
#include "space_page.h"

int main(int argc, char* argv[])
{
	Config* config = Config::Instance();
	config->loadArgs(argc, argv);
	config->setFile("$HOME/.lightshifters");
	
	PageManager* man = PageManager::Instance();
	man->setCaption(L"Lightshifters SiO2 []");
	man->graphics().setResolution(
		config->get<unsigned>("screenWidth", 1024),
		config->get<unsigned>("screenHeight", 768)
			);
	man->load<SpacePage>();
	man->show();
	return 0;
}