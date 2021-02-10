//
// Created by Qsymond on 04.02.2021.
//

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
# include "Server.hpp"

# define CONFIG_FILE_DEFAULT_PATH "/web.conf"

class Webserv
{
private:
	std::vector<Server *> servers;
	fd_set read_set;
	fd_set write_set;
	int maxFd;
	Config serverConfig;
	bool isServerRunning;
public:
	Webserv();

	Webserv(const std::string &config_file_path);

	void addServer(Server *server);

	int getServerSize() const;

	Server *getServer(int i);

	void setToReadFDSet(std::list<int> &clientsFD);

	void updateMaxFD();

	const int &getMaxFD() const;

	void run();

	static void stop(int n);
};

#endif
