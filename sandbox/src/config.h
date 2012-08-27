#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <iostream>

class Config {

	static Config* ms_instance;
	
	std::string m_sFilename;
	std::map<std::string, std::string> m_mPermanent, m_mTemp;

private:
	Config(const Config& rhs);
	Config& operator=(const Config& rhs);

public:
	static Config* Instance();
	static void Release();
	
	void loadArgs(int argc, char* argv[]);
	void setFile(std::string filename);
	template<typename T> T get(std::string key, T def);

private:
	void parse(std::string, bool permanent);
	Config();
	~Config();
};

#include <sstream>
#include <Gosu/Input.hpp>

template<> inline Gosu::Button Config::get(std::string key, Gosu::Button def)
{
	return Gosu::Button(get(key, def.id()));
}

template<typename T> inline T Config::get(std::string key, T def)
{
	std::map<std::string, std::string>::const_iterator it = m_mTemp.find(key);
	std::string value;
	if (it != m_mTemp.end()) {
		value = it->second;
	} else {
		it = m_mPermanent.find(key);
		if (it != m_mPermanent.end()) {
			value = it->second;
		} else {
			std::stringstream ss;
			ss << def;
			m_mPermanent[key] = ss.str();
			return def;
		}
	}
	
	std::stringstream ss(value);
	T ret;
	if (ss >> ret) {
		return ret;
	}
	std::stringstream ss2;
	ss2 << def;
	std::cerr << "Invalid value for \"" << key << "\": " << value << " ... using default: " << ss2.str() << std::endl;
	return def;
}

#endif // CONFIG_H
