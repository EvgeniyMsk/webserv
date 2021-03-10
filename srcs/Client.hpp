#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/wait.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include "Utils.hpp"

#define TMP_PATH 	"/tmp/cgi.tmp"

class Client
{
	friend class Server;

	friend class HTTP;

public:

	int fd;
	int read_fd;
	int write_fd;

	void readFile();

	void writeFile();
	std::map<std::string, std::string> clientConfig;
private:
	int port;
	int status;
	int cgi_pid;
	int tmp_fd;
	char *buffer;
	fd_set *read_set;
	fd_set *write_set;
	Request request;
	Response response;
	std::string ip;
	std::string lastDate;
	std::string textResponse;

	w_chunk chunk;

public:
	Client(int clientSocket, fd_set *new_read_set, fd_set *new_write_set, struct sockaddr_in info);

	~Client();

	void setReadState(bool state);

	void setWriteState(bool state);

	void setFileToRead(bool state);

	void setFileToWrite(bool state);

	void setToStandBy();
};

#endif
