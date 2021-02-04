//
// Created by Qsymond on 04.02.2021.
//

#ifndef WEBSERV_CONFIG_HPP
#define WEBSERV_CONFIG_HPP

#include <string>
class Config
{
	typedef std::string string;
	public:
	string	ipAddress;
	int		port;
	string 	serverName;
	size_t 	bufferSize;
	public:
	Config();
	virtual ~Config();
};


#endif //WEBSERV_CONFIG_HPP
