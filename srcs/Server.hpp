#ifndef SERVER_HPP
#define SERVER_HPP
#include <arpa/inet.h>
#include <string>
#include <vector>
#include "Config.hpp"

class Server
{
private:
	size_t		port;
	long int	maxFileSize;
	std::string host;
	std::string	serverName;
	std::string	errorPage;
	std::string	root;
	std::string	autoindex;
	int fd;
	std::string	socketFd;
	struct sockaddr_in addr;
	std::vector<std::string> indexes;
	std::vector<Location *> locations;
public:
	Server(/* args */);
	~Server();
};

Server::Server(/* args */)
{
}

Server::~Server()
{
}


#endif