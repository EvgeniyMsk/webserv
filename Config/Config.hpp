//
// Created by Qsymond on 18.02.2021.
//

#ifndef WEBSERV_CONFIG_HPP
#define WEBSERV_CONFIG_HPP
#include <string>

class Config
{
private:
	std::string serverName;
	int port;
public:
	Config(const std::string &serverName, int port);

	virtual ~Config();

	const std::string &getServerName() const;

	int getPort() const;
};


#endif //WEBSERV_CONFIG_HPP
