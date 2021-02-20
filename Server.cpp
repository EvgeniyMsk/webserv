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

int get_time(void)
{
    struct timeval	current;

    gettimeofday(&current, NULL);
    return ((int)(((current.tv_sec) * 1000) + ((current.tv_usec) / 1000)));
}

void set_time(std::map<int, int> &time, std::list<int>::iterator it, std::list<int>::iterator const& ite) {
    while (it != ite) {
        time[*it] = get_time();
        ++it;
    }
}

void Server::handleRequests(fd_set* globalReadSetPtr)
{
	char buf[1024];
	int bytes_read;
	std::list<int>::iterator it = readClient.begin();
	std::map<int, int> time;
    set_time(time, it, readClient.end());
	while (it != readClient.end())
	{
		if (FD_ISSET(*it, globalReadSetPtr))
		{
			// Поступили данные от клиента, читаем их
			bytes_read = recv(*it, buf, 1024, 0);

			if (bytes_read == 0)
			{
				// Соединение разорвано, удаляем сокет из множества
				readError(it);
				continue;
			}

			else if (bytes_read < 0)
                bytes_read = 0;
            else
                time[*it] = get_time();
            buf[bytes_read] = '\0';
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

void Server::handleResponses(fd_set* globalWriteSetPtr)
{
	std::list<int>::iterator it = writeClient.begin();
	int fd;
	while (it != writeClient.end())
	{
		fd = *it;
		if (FD_ISSET(fd, globalWriteSetPtr))
		{
			Request* request = clientRequest[fd];
			//ты начал парсить, нужно сюда видимо вставить будет

			Response response(request, fd);
			response.generateResponse();
			response.sendResponse();

			close(fd);
			clientRequest.erase(fd);
			it = writeClient.erase(it);
		} else
			++it;
	}
}

void Server::processConnections(fd_set* globalReadSetPtr, fd_set* globalWriteSetPtr)
{
	handleRequests(globalReadSetPtr);
	handleResponses(globalWriteSetPtr);
}

void Server::updateMaxFd()
{
    int maxTmp = list;
    if (!readClient.empty())
        maxTmp = std::max(maxTmp, *std::max_element(readClient.begin(), readClient.end()));
	 if (!writeClient.empty())
    	maxTmp = std::max(maxTmp, *std::max_element(writeClient.begin(), writeClient.end()));
    maxFd = maxTmp;
}

std::list<int> &Server::getReadClients()
{
	return (readClient);
}

void        Server::readError(std::list<int>::iterator & it)
{
    close(*it);
    delete clientRequest[*it];
    it = writeClient.erase(it);
}