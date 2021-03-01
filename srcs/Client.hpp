#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include "Request.hpp"

# ifndef CONNECTION_LOGS
#  define CONNECTION_LOGS 0
# endif
#define	BUFFERSIZE 4096

class Server;
struct Client
{
	Server *server;
	int clientSocket;
	int port;
	bool isConnected;
	struct sockaddr_in addr;
	socklen_t size;
	std::string req;
	std::string	host;
	std::string ipAddress;
	time_t lastRequest;
	request_s request;

	explicit Client(Server *x);

	~Client();

	int receiveRequest();

	void resetTimeout();

	void sendReply(const char *msg, request_s &request) const;

	void checkTimeout();

	void reset(const std::string &);

	static void breakOnSIGPIPE(int);

	private:
	Client();
};

#endif
