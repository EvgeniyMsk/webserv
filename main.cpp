#include "src/Server.hpp"
#include "src/Webserv.hpp"
#include <signal.h>

int main(int argc, char **argv)
{
	Webserv webserv;
	webserv.run();
	return 0;
}