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
#include <fcntl.h>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <map>
#include <algorithm>
#include "signal.h"
namespace utils
{
	void exitWithLog();

	uint16_t getSwapped(uint16_t port);
}

#endif
