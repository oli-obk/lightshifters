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
    uint16_t host_port = config->get<uint16_t>("host_port", 50042);
    bool server = config->get<bool>("host", true);
    if (server) {
        try {
            man->load<ServerPage>(host_port);
        } catch (...) {
            server = false;
        }
    }
    if (!server) {
        std::cout << "loading client" << std::endl;
        man->load<ClientPage>(config->get<std::string>("connectTo", "localhost"), config->get<uint16_t>("port", 50042), host_port);
    }
	man->show();
	man->Release();
	config->Release();
	return 0;
}