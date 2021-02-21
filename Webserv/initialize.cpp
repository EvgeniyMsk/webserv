//
// Created by Qsymond on 20.02.2021.
//
#include "Webserv.hpp"

Webserv::Webserv(std::vector<Config> conf, int size)
{
	serverSize = size;
	config = conf;
	ports = (int *)malloc(sizeof(int) * size + 1);
	sockets = (int *)malloc(sizeof(int) * size + 1);
	addresses = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in) * size + 1);
	maxFD = 0;
	memset((char *) &m_read_set, 0, sizeof(m_read_set));
	memset((char *) &m_write_set, 0, sizeof(m_write_set));
	memset((char *) &w_read_set, 0, sizeof(w_read_set));
	memset((char *) &w_write_set, 0, sizeof(w_write_set));
	for (int i = 0; i < size; i++)
	{
		memset((int *) &ports[i], 0, sizeof(ports[i]));
		memset((int *) &sockets[i], 0, sizeof(sockets[i]));
		memset((char *) &addresses[i], 0, sizeof(addresses[i]));
	}
}

void Webserv::set_non_block(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		strerror(errno);
	}
}

void Webserv::build_fd_set()
{
	FD_ZERO(&m_read_set);
	for (int i = 0; i < serverSize; i++)
	{
		FD_SET(sockets[i], &m_read_set);
		maxFD = std::max(sockets[i], maxFD);
	}
	FD_ZERO(&m_write_set);
	for (int i = 0; i < serverSize; i++)
	{
		FD_SET(sockets[i], &m_write_set);
		maxFD = std::max(sockets[i], maxFD);
	}

}

void Webserv::initServers()
{
	signal(SIGINT, &Webserv::exiting);
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
	for (int i = 0; i < serverSize; i++)
		ports[i] = config[i].getPort();

	for (int i = 0; i < serverSize; i++)
	{
		sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (sockets[i] < 0)
			utils::exitWithLog();
	}

	int reuse = 1;
	for (int i = 0; i < serverSize; i++)
	{
		setsockopt(sockets[i], SOL_SOCKET, SO_REUSEADDR, &reuse,sizeof(reuse));
	}


	for (int i = 0; i < serverSize; i++)
		set_non_block(sockets[i]);

	int swapped;
	for (int i = 0; i < serverSize; i++)
	{
		addresses[i].sin_family = AF_INET;
		swapped = (ports[i] >> 8) | (ports[i] << 8);
		addresses[i].sin_port = swapped;
		addresses[i].sin_addr.s_addr = inet_addr(LOCALHOST);
	}

	for (int i = 0; i < serverSize; i++)
		if (bind(sockets[i], (struct sockaddr *) &addresses[i], sizeof(addresses[i])) < 0)
			utils::exitWithLog();

	for (int i = 0; i < serverSize; i++)
		if (listen(sockets[i], SOMAXCONN) < 0)
			utils::exitWithLog();

	build_fd_set();

	std::cout << "Server initializes successfully!" << std::endl;
}