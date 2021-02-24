#include "Web.hpp"

Web::Web(const std::string& config_file_path)
{
    if (config_file_path == CONFIG_FILE_DEFAULT_PATH)
	{
        Server* one = new Server(8080);
        Server* two = new Server(8081);
        addServer(one);
        addServer(two);
    }
	else
	{
        conf = Config(config_file_path);
    }
    std::cout << "Server started" << std::endl;
}


void Web::addServer(Server* server)
{
    servers.push_back(server);
}

int Web::getServersCount() const
{
	return (servers.size());
}

void Web::setToReadFDSet(std::list<int>& clientsFD)
{
    for(std::list<int>::iterator it = clientsFD.begin(); it != clientsFD.end(); it++)
        FD_SET(*it, &readSet);
}

void Web::updateMaxFD()
{
    maxFd = 0;
    for(std::vector<Server*>::iterator it = servers.begin(); it != servers.end(); it++) {
        int tmp = (*it)->getMaxFd();
        if (tmp > maxFd)
            maxFd = tmp;
    }
}

Server* Web::getServerByPosition(int i)
{
    return (servers[i]);
}

fd_set* Web::getReadSetPtr()
{
	return (&readSet);
}

fd_set* Web::getWriteSetPtr()
{
	return (&writeSet);
}

 const int &Web::getMaxFD() const
 {
	 return (maxFd);
 }

void Web::serveConnections()
{
	while(TRUE)
	{
		FD_ZERO(getReadSetPtr());
		for (int i = 0; i < getServersCount(); i++)
		{
			Server *server = getServerByPosition(i);
			FD_SET(server->getList(), getReadSetPtr());
			setToReadFDSet(server->getReadClients());
			server->updateMaxFd();
		}
		updateMaxFD();

		// ожидание события в одном из сокетов
		if (select(getMaxFD() + 1, getReadSetPtr(), getWriteSetPtr(), NULL, NULL) < 0)
			utils::exitWithLog();

		// определяем тип события и выполняем соответствующие действия
		for (int i = 0; i < getServersCount(); i++)
		{
			Server *server = getServerByPosition(i);
			if (FD_ISSET(server->getList(), getReadSetPtr())) // новый запрос на соединение, accept
				server->acceptConnection();
			server->processConnections(getReadSetPtr(), getWriteSetPtr());
		}
	}
}

void Web::stop()
{
	return ;
}
