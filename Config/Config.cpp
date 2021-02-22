//
// Created by Qsymond on 18.02.2021.
//

#include "Config.hpp"

Config::Config(const std::string &serverName, int port) : serverName(serverName), port(port)
{}

Config::~Config()
{

}

const std::string &Config::getServerName() const
{
	return serverName;
}

int Config::getPort() const
{
	return port;
}
