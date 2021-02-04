//
// Created by Qsymond on 04.02.2021.
//

#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include <cstdlib>
#include <iostream>
#include "unistd.h"
#include "fcntl.h"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include "../headers/defines.hpp"
#include <sys/time.h>
#include "Config.hpp"

#define TRUE             1
#define FALSE            0
class Webserv
{
private:
	Config	serverConfig;
	int			end_server;
	int    listen_sd;
	struct sockaddr_in addr;
	struct timeval      timeout;
	fd_set              master_set, working_set;
	int    	max_sd, new_sd;
	int 	rc;
public:
	Webserv(Config *config);

	virtual ~Webserv();

	void init();

	void run();

	std::string getCurrentTime() const;
};


#endif
