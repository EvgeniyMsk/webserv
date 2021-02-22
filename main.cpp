#include <iostream>
#include "Webserv/Webserv.hpp"
int main()
{
	Config conf1("Server_1", 1111);
	Config conf2("Server_2", 2222);
	Config conf3("Server_3", 3333);
	Config conf4("Server_4", 4444);
	std::vector<Config> config;
	config.push_back(conf1);
	config.push_back(conf2);
	config.push_back(conf3);
	config.push_back(conf4);
	Webserv webserv(config, config.size());
	webserv.initServers();
	webserv.run();
	return 0;
}