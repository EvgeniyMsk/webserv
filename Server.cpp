#include "Server.hpp"

Server::Server(int newPort) : port(newPort)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	list = socket(AF_INET, SOCK_STREAM, 0);
	if (list < 0)
		utils::exitWithLog();
	if (fcntl(list, F_SETFL, O_NONBLOCK) < 0) //превращает сокет в неблокирующий
		utils::exitWithLog();
	int optval = 1;
	if (setsockopt(list, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		utils::exitWithLog();
	if (bind(list, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		utils::exitWithLog();
	if (listen(list, SOMAXCONN) < 0)
		utils::exitWithLog();
};

const int &Server::getList() const
{
	return (list);
}

void Server::acceptConnection()
{
	int sock = accept(list, NULL, NULL);
	if(sock < 0)
		utils::exitWithLog();
	if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
		utils::exitWithLog();
	readClient.push_back(sock);
}

void Server::handleRequests(fd_set* globalReadSetPtr)
{
	char buf[1024];
	int bytes_read;
	std::list<int>::iterator it = readClient.begin();

	while (it != readClient.end())
	{
		if (FD_ISSET(*it, globalReadSetPtr))
		{
			// Поступили данные от клиента, читаем их
			bytes_read = recv(*it, buf, 1024, 0);

			if (bytes_read <= 0)
			{
				// Соединение разорвано, удаляем сокет из множества
				close(*it);
				it = readClient.erase(it);
				continue;
			}

			// Мы знаем фактический размер полученных данных, поэтому ставим метку конца строки в буфере запроса
			buf[bytes_read] = '\0';

			std::string tmp = buf;
			Request* tmpRequest = new Request(tmp);
			clientRequest[*it] = tmpRequest;
			it = readClient.erase(it);
		} else
			it++;
	}
}

const int &Server::getMaxFd() const
{
	return (maxFd);
}

void Server::processConnections(fd_set* globalReadSetPtr)
{
	handleRequests(globalReadSetPtr);
}

void Server::updateMaxFd()
{
    int maxTmp = list;
    if (!readClient.empty())
        maxTmp = std::max(maxTmp, *std::max_element(readClient.begin(), readClient.end()));
    maxFd = maxTmp;
}

std::list<int> &Server::getReadClients()
{
	return (readClient);
}
