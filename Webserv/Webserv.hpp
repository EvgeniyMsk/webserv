//
// Created by Qsymond on 18.02.2021.
//

#ifndef WEBSERV_WEBSERV_HPP
#define WEBSERV_WEBSERV_HPP

#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>
#include "../Config/Config.hpp"
#include "../Request/Request.hpp"
#include "stdlib.h"
#include <vector>
#include "../Utils/Utils.hpp"
#include <sstream>
#include "unistd.h"
#include "signal.h"
#include <signal.h>
#include "Client.hpp"
#define LOCALHOST "127.0.0.1"
#define BUFFER_SIZE 10000
#define TRUE             1
#define FALSE            0
#define SERVER_PORT  1111
class Webserv
{
private:
	std::vector<Config> config;
	int			serverSize;
	struct		sockaddr_in *addresses;
	int			maxFD;
	fd_set		m_read_set;	//master read_set
	fd_set		m_write_set;//master write_set
	fd_set		w_read_set;	//working read_set
	fd_set		w_write_set;//working write_set
	int			*ports;
	int			*sockets;
	std::vector<Request*> requests;
	std::vector<Client *> clients;
	bool		m_close;
	timeval timeout;
public:
	Webserv(std::vector<Config> conf, int size);

	virtual ~Webserv();

	void initServers();

	void run();

	void set_non_block(int fd);

	void send_data(int fd);

	void close_conn(int fd);

	void accept_connection(int fd);

	void receiveData(int client_sock);

	void build_fd_set();

//	void readRequest(int fd);

	bool isServerSocket(int fd);

	void clean();

	void doCGI(int fd);

	static void exiting(int n);
};


#endif //WEBSERV_WEBSERV_HPP
