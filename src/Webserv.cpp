#include "Webserv.hpp"

Webserv::Webserv()
{
	addServer(new Server(1111, "Server1"));
	addServer(new Server(2222, "Server2"));
	addServer(new Server(3333, "Server3"));
	addServer(new Server(4444, "Server4"));
}

Webserv::Webserv(const std::string& config_file_path)
{
	if (config_file_path == CONFIG_FILE_DEFAULT_PATH)
	{
		Server *serv1 = new Server(1111, "Server1");
		Server *serv2= new Server(2222, "Server2");
		Server *serv3 = new Server(3333, "Server3");
		Server *serv4= new Server(4444, "Server4");
		addServer(serv1);
		addServer(serv2);
		addServer(serv3);
		addServer(serv4);
	}
	else
	{
		serverConfig = Config(config_file_path);
	}
	std::cout << "Server started" << std::endl;
}


void Webserv::addServer(Server* server)
{
	servers.push_back(server);
}

int Webserv::getServerSize() const
{
	return (servers.size());
}

void Webserv::update_ft_sets()
{
	for(std::vector<Server *>::iterator servs = servers.begin(); servs != servers.end(); servs++)
	{
		FD_SET((*servs)->getServerSocket(), &read_set);
		for (std::vector<Client *>::iterator clients = (*servs)->getReadClients().begin(); clients != (*servs)->getReadClients().end(); clients++)
			FD_SET((*clients)->getClientSocket(), &read_set);
		(*servs)->updateMaxFd();
	}

}

void Webserv::updateMaxFD()
{
	maxFd = 0;
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		int tmp = (*it)->getMaxFd();
		if (tmp > maxFd)
			maxFd = tmp;
	}
}

Server* Webserv::getServer(int i)
{
	return (servers[i]);
}

const int &Webserv::getMaxFD() const
{
	return (maxFd);
}

void Webserv::run()
{
	isServerRunning = true;
	signal(SIGTERM, &Webserv::stop);
	signal(SIGINT, &Webserv::stop);
	std::cout << "Webserv started" << std::endl;
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
		std::cout << (*it)->getServerName() << " " << inet_ntoa((*it)->getServerAddress().sin_addr) << ":" << (*it)->getPort() << " has started!\n";

	while (isServerRunning)
	{
		maxFd = 0;
		FD_ZERO(&read_set);
		FD_ZERO(&write_set);
		update_ft_sets();
		updateMaxFD();
		if (select(getMaxFD() + 1, &read_set, &write_set, nullptr, nullptr) < 0)
			utils::exitWithLog();
		for (int i = 0; i < getServerSize(); i++)
		{
			Server *server = getServer(i);
			if (FD_ISSET(server->getServerSocket(), &read_set))
				server->acceptConnection();
			server->readRequest(read_set);
			server->writeResponse(write_set);
		}
	}
}

void Webserv::stop(int n)
{
	(void)n;
	std::cout << "Server stopped..." << std::endl;
	return;
}


