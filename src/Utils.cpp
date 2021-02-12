#include "Utils.hpp"

void utils::exitWithLog()
{
	std::cout << std::strerror(errno) << std::endl;
	exit(errno);
}

unsigned short utils::getSwapped(uint16_t port)
{
	return (uint16_t) (((uint16_t) port >> 8) | ((uint16_t) port << 8));
}
