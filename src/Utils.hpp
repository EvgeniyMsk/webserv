//
// Created by Qsymond on 10.02.2021.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include "stdlib.h"
#include <list>
#include <map>
#include <algorithm>
#include "signal.h"
#include <fstream>


#define LOCALHOST "127.0.0.1"
#define REFERER_STR "Referer: "
#define USER_AGENT_STR "User-Agent: "
#define TRANSFER_EN_STR "Transfer-Encoding: "
#define AUTH_STR "Authorization: "
#define ACCEPT_EN_STR "Accept-Encoding: "
#define CONTENT_T_STR "Content-Type: "
#define CONTENT_L_STR "Content-Length: "
#define ACCEPT_CHAR_STR "Accept-Charset: "
#define DATE_STR "Date: "
#define ACCEPT_LAN_STR "Accept-Language: "

namespace utils
{
	void exitWithLog();

	uint16_t getSwapped(uint16_t port);
}

#endif
