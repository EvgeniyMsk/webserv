//
// Created by Qsymond on 10.02.2021.
//

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP
# include "Utils.hpp"
# include "Request.hpp"
# include "Config.hpp"


#include <arpa/inet.h>
class Server
{
private:
	int 						serverSocket;
	struct sockaddr_in 			serverAddress;
	int 						port;
	std::string 				serverName;
	std::list<int> 				clientSockets;
	std::map<int, Request *>	clientRequest;
	int 						maxFd;
	size_t 						BUFFER_SIZE;
public:
	Server() {};

	Server(int newPort, std::string const &serverName);

	virtual ~Server()
	{};

	const int &getServerSocket() const;

	void updateMaxFd();

	const int &getMaxFd() const;

	void acceptConnection();

	void readRequest(fd_set *globalReadSetPtr);

	std::list<int> &getReadClients();

	void setNonBlock(int fd);

	int receiveData(int client_sock, std::string &str);

	const sockaddr_in &getServerAddress() const;

	int getPort() const;

	const std::string &getServerName() const;
};


#endif //WEBSERV_SERVER_HPP
