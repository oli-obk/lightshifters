#include "config.h"
#include <fstream>

Config* Config::ms_instance = 0;

Config::Config()
{
}

Config::~Config()
{
	std::ofstream out(m_sFilename);
	for(auto it = m_mPermanent.begin(); it != m_mPermanent.end(); it++) {
		out << it->first << "=" << it->second << std::endl;
	}
}

Config* Config::Instance()
{
	if(ms_instance == 0){
		ms_instance = new Config();
	}
	return ms_instance;
}

void Config::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}


void Config::loadArgs(int argc, char* argv [])
{
	for(int i = 1; i < argc; i++) {
		std::string line(argv[i]);
		parse(line, false);
	}
}

void Config::parse(std::string line, bool permanent)
{
	if (line.length() < 3) return;
	size_t pos = line.find('=');
	if (pos == 0) return;
	if (pos == std::string::npos) return;
	if (pos == line.length() -1) return;
	std::string key = line.substr(0, pos);
	std::string val = line.substr(pos+1);
	if (permanent) {
		m_mPermanent[key] = val;
	} else {
		m_mTemp[key] = val;
	}
	std::cout << (permanent?"perm: ":"temp: ") << line << std::endl;
}

void Config::setFile(std::string filename)
{
	m_sFilename = filename;
	std::ifstream in(filename);
	std::string line;
	while(in) {
		std::getline(in, line);
		parse(line, true);
	}
}

