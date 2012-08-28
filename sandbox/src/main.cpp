#include "client_page.h"
#include "server_page.h"
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
    try {
        man->load<ServerPage>(config->get<uint16_t>("port", 50042));
    } catch (...) {
        std::cout << "loading client" << std::endl;
        man->load<ClientPage>(config->get<std::string>("connectTo", "localhost"), config->get<uint16_t>("port", 50042));
    }
	man->show();
	man->Release();
	config->Release();
	return 0;
}