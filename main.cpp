#include <cstdlib>
#include <iostream>
#include "src/Webserv.hpp"
#include "headers/defines.hpp"

void signalHandler( int signum ) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";
//	close(mySocket);
//	close(client_socket);
//	close(result);
	exit(signum);
}


int main()
{
	Webserv webserv = Webserv("127.0.0.1", 8080);
	webserv.init();
//	std::cout << webserv.getCurrentTime();
}