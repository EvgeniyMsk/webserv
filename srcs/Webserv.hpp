#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include "Server.hpp"
#include <map>
#include <sys/stat.h>

class Webserv
{
	fd_set w_read_set;
	fd_set w_write_set;
	fd_set m_read_set;
	fd_set m_write_set;
	std::map<int, int> connections;
	std::vector<Server *> servers;
	char *configPath;

public:
	Webserv();

	explicit Webserv(char *configPath);

	Webserv(const Webserv &obj);

	Webserv &operator=(const Webserv &obj);

	virtual ~Webserv();

	void startListening();

	void startServer();

	void loadConfiguration();

	void stopServer();

	static void signalServer(int n);

	static bool checkIfEnded(const std::string &request);

	int getMaxFD();
};

#endif
