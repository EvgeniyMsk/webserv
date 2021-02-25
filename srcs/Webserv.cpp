#include "utils.h"
#include "Webserv.hpp"

extern bool					g_state;

Webserv::Webserv() : serverSocket(-1), maxFD(-1), port(-1)
{
	memset(&address, 0, sizeof(address));
}

Webserv::~Webserv()
{
	Client		*client = NULL;

	if (serverSocket != -1)
	{
		for (std::vector<Client*>::iterator it(clients.begin()); it != clients.end(); ++it)
		{
			client = *it;
			*it = NULL;
			if (client)
				delete client;
		}
		while (!_tmp_clients.empty())
		{
			close(_tmp_clients.front());
			_tmp_clients.pop();
		}
		clients.clear();
		close(serverSocket);
		FD_CLR(serverSocket, m_read_set);
		g_logger.log("[" + std::to_string(port) + "] " + "closed", LOW);
	}
}

int		Webserv::getMaxFd()
{
	Client	*client;

	for (std::vector<Client*>::iterator it(clients.begin()); it != clients.end(); ++it)
	{
		client = *it;
		if (client->read_fd > maxFD)
			maxFD = client->read_fd;
		if (client->write_fd > maxFD)
			maxFD = client->write_fd;
	}
	return (maxFD);
}

int		Webserv::getServerSocket() const
{
	return (serverSocket);
}

int		Webserv::getOpenFd()
{
	int 	nb = 0;
	Client	*client;

	for (std::vector<Client*>::iterator it(clients.begin()); it != clients.end(); ++it)
	{
		client = *it;
		nb += 1;
		if (client->read_fd != -1)
			nb += 1;
		if (client->write_fd != -1)
			nb += 1;
	}
	nb += _tmp_clients.size();
	return (nb);
}

void	Webserv::init(fd_set *readSet, fd_set *writeSet, fd_set *rSet, fd_set *wSet)
{
	int				yes = 1;
	std::string		to_parse;
	std::string		host;

	w_read_set = readSet;
	w_write_set = writeSet;
	m_write_set = wSet;
	m_read_set = rSet;

	to_parse = s_config[0]["server|"]["listen"];
	errno = 0;
	if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw(ServerException("socket()", std::string(strerror(errno))));
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw(ServerException("setsockopt()", std::string(strerror(errno))));
    if (to_parse.find(":") != std::string::npos)
    {
    	host = to_parse.substr(0, to_parse.find(":"));
    	if ((port = atoi(to_parse.substr(to_parse.find(":") + 1).c_str())) < 0)
			throw(ServerException("Wrong port", std::to_string(port)));
		address.sin_addr.s_addr = inet_addr(host.c_str());
		address.sin_port = htons(port);
    }
    else
    {
		address.sin_addr.s_addr = INADDR_ANY;
		if ((port = atoi(to_parse.c_str())) < 0)
			throw(ServerException("Wrong port", std::to_string(port)));
		address.sin_port = htons(port);
    }
	address.sin_family = AF_INET;
	if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) == -1)
		throw(ServerException("bind()", std::string(strerror(errno))));
    if (listen(serverSocket, 256) == -1)
		throw(ServerException("listen()", std::string(strerror(errno))));
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw(ServerException("fcntl()", std::string(strerror(errno))));
	FD_SET(serverSocket, m_read_set);
    maxFD = serverSocket;
    g_logger.log("[" + std::to_string(port) + "] " + "listening...", LOW);
}

void	Webserv::refuseConnection()
{
	int 				fd = -1;
	struct sockaddr_in	info;
	socklen_t			len;

	errno = 0;
	len = sizeof(struct sockaddr);
	if ((fd = accept(serverSocket, (struct sockaddr *)&info, &len)) == -1)
		throw(ServerException("accept()", std::string(strerror(errno))));
	if (_tmp_clients.size() < 10)
	{
		_tmp_clients.push(fd);
		FD_SET(fd, m_write_set);
	}
	else
		close(fd);
}

void	Webserv::acceptConnection()
{
	int 				fd = -1;
	struct sockaddr_in	info;
	socklen_t			len;
	Client				*newOne = NULL;

	memset(&info, 0, sizeof(struct sockaddr));
	errno = 0;
	len = sizeof(struct sockaddr);
	if ((fd = accept(serverSocket, (struct sockaddr *)&info, &len)) == -1)
		throw(ServerException("accept()", std::string(strerror(errno))));
	if (fd > maxFD)
		maxFD = fd;
	newOne = new Client(fd, m_read_set, m_write_set, info);
	clients.push_back(newOne);
	g_logger.log("[" + std::to_string(port) + "] " + "connected clients: " + std::to_string(clients.size()), LOW);
}

int		Webserv::readRequest(std::vector<Client*>::iterator it)
{
	int 		bytes;
	int			ret;
	Client		*client = NULL;
	std::string	log;

	client = *it;
	bytes = strlen(client->rBuf);
	ret = read(client->fd, client->rBuf + bytes, BUFFER_SIZE - bytes);
	bytes += ret;
	if (ret > 0)
	{
		client->rBuf[bytes] = '\0';
		if (strstr(client->rBuf, "\r\n\r\n") != NULL
			&& client->status != Client::BODYPARSING)
		{
			log = "REQUEST:\n";
			log += client->rBuf;
			g_logger.log(log, HIGH);
			client->last_date = ft::getDate();
			_handler.parseRequest(*client, s_config);
			client->setWriteState(true);
		}
		if (client->status == Client::BODYPARSING)
			_handler.parseBody(*client);
		return (1);
	}
	else
	{
		*it = NULL;
		clients.erase(it);
		if (client)
			delete client;
		g_logger.log("[" + std::to_string(port) + "] " + "connected clients: " + std::to_string(clients.size()), LOW);
		return (0);
	}
}

int		Webserv::writeResponse(std::vector<Client*>::iterator it)
{
	unsigned long	bytes;
	std::string		tmp;
	std::string		log;
	Client			*client = NULL;

	client = *it;
	switch (client->status)
	{
		case Client::RESPONSE:
			log = "RESPONSE:\n";
			log += client->response.substr(0, 128);
			g_logger.log(log, HIGH);
			bytes = write(client->fd, client->response.c_str(), client->response.size());
			if (bytes < client->response.size())
				client->response = client->response.substr(bytes);
			else
			{
				client->response.clear();
				client->setToStandBy();
			}
			client->last_date = ft::getDate();
			break ;
		case Client::STANDBY:
			if (getTimeDiff(client->last_date) >= TIMEOUT)
				client->status = Client::DONE;
			break ;
		case Client::DONE:
			delete client;
			clients.erase(it);
			g_logger.log("[" + std::to_string(port) + "] " + "connected clients: " + std::to_string(clients.size()), LOW);
			return (0);
		default:
			_handler.dispatcher(*client);
	}
	return (1);
}

void	Webserv::send503(int fd)
{
	Response		response;
	std::string		str;
	int				ret = 0;

	response.version = "HTTP/1.1";
	response.status_code = UNAVAILABLE;
	response.headers["Retry-After"] = RETRY;
	response.headers["Date"] = ft::getDate();
	response.headers["Server"] = "webserv";
	response.body = UNAVAILABLE;
	response.headers["Content-Length"] = std::to_string(response.body.size());
	std::map<std::string, std::string>::const_iterator b = response.headers.begin();
	str = response.version + " " + response.status_code + "\r\n";
	while (b != response.headers.end())
	{
		if (b->second != "")
			str += b->first + ": " + b->second + "\r\n";
		++b;
	}
	str += "\r\n";
	str += response.body;
	ret = write(fd, str.c_str(), str.size());
	if (ret >= -1)
	{
		close(fd);
		FD_CLR(fd, m_write_set);
		_tmp_clients.pop();
	}
	g_logger.log("[" + std::to_string(port) + "] " + "connection refused, sent 503", LOW);
}


int		Webserv::getTimeDiff(std::string start)
{
	struct tm		start_tm;
	struct tm		*now_tm;
	struct timeval	time;
	int				result;

	strptime(start.c_str(), "%a, %d %b %Y %T", &start_tm);
	gettimeofday(&time, NULL);
	now_tm = localtime(&time.tv_sec);
	result = (now_tm->tm_hour - start_tm.tm_hour) * 3600;
	result += (now_tm->tm_min - start_tm.tm_min) * 60;
	result += (now_tm->tm_sec - start_tm.tm_sec);
	return (result);
}

Webserv::ServerException::ServerException(void)
{
	this->error = "Undefined Server Exception";	
}

Webserv::ServerException::ServerException(std::string function, std::string error)
{
	this->error = function + ": " + error;
}

Webserv::ServerException::~ServerException(void) throw() {}

const char			*Webserv::ServerException::what(void) const throw()
{
	return (this->error.c_str());
}
