#ifndef Server_HPP
#define Server_HPP

#include <queue>
#include "HTTP.hpp"

#define TIMEOUT 10
#define RETRY	"25"

class Server
{
	friend class Webserv;

private:
	int serverSocket;
	int maxFd;
	int port;
	struct sockaddr_in addr;
	fd_set *_readSet;
	fd_set *_writeSet;
	fd_set *_rSet;
	fd_set *_wSet;
	HTTP http;


public:
	std::vector<Client *> clients;
	std::queue<int> tmpClients;
	std::vector<Config> config;
	Server();

	~Server();

	int getMaxFd();

	int getFd() const;

	int getOpenFd();

	void init(fd_set *readSet, fd_set *writeSet, fd_set *rSet, fd_set *wSet);

	void refuseConnection();

	void acceptConnection();

	int readRequest(std::vector<Client *>::iterator it);

	int writeResponse(std::vector<Client *>::iterator it);

	void send503(int fd);

	class ServerException : public std::exception
	{
	private:
		std::string function;
		std::string error;

	public:
		ServerException();

		ServerException(std::string function, std::string error);

		virtual    ~ServerException() throw();

		virtual const char *what() const throw();
	};

private:
	int getTimeDiff(std::string start);

};

#endif
