#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <csignal>
#include "../headers/defines.hpp"
#include "../headers/ResponseHeaders.hpp"
#include <sys/time.h>
class Webserv
{
	typedef std::string string;
	private:
	string ipAddress;
	int port;
	int serverSocket;
	string serverName;
	ResponseHeaders responseHeaders;
public:
	const string &getServerName() const;

public:
	Webserv(const std::string &ipAddress, int port);

	virtual ~Webserv();

	const std::string &getIpAddress() const;

	void setIpAddress(const std::string &ipAddress);

	int getPort() const;

	void setPort(int port);

	std::string getCurrentTime() const;

	void init();

};


#endif
