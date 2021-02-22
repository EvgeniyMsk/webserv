//
// Created by Qsymond on 18.02.2021.
//

#include "Utils.hpp"

void utils::exitWithLog()
{
	std::cout << std::strerror(errno) << std::endl;
	exit(errno);
}