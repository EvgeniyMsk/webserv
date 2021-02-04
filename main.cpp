#include "src/Webserv.hpp"


#define SERVER_PORT  12345

#define TRUE             1
#define FALSE            0


int main (int argc, char *argv[])
{
	Webserv webserv;
	webserv.init();
	webserv.run();
	return (0);
}