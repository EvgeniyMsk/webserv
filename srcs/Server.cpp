#include "Server.hpp"

Server::Server() : _fd(-1), _maxFd(-1), _port(-1)
{
	memset(&_info, 0, sizeof(_info));
}

Server::~Server()
{
	Client *client = nullptr;

	if (_fd != -1)
	{
		for (std::vector<Client *>::iterator it(_clients.begin()); it != _clients.end(); ++it)
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
		_clients.clear();
		close(_fd);
		FD_CLR(_fd, _rSet);
		utils::showMessage("[" + std::to_string(_port) + "] " + "closed");
	}
}

int Server::getMaxFd()
{
	Client *client;

	for (std::vector<Client *>::iterator it(_clients.begin()); it != _clients.end(); ++it)
	{
		client = *it;
		if (client->read_fd > _maxFd)
			_maxFd = client->read_fd;
		if (client->write_fd > _maxFd)
			_maxFd = client->write_fd;
	}
	return (_maxFd);
}

int Server::getFd() const
{
	return (_fd);
}

int Server::getOpenFd()
{
	int nb = 0;
	Client *client;

	for (std::vector<Client *>::iterator it(_clients.begin()); it != _clients.end(); ++it)
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

void Server::init(fd_set *readSet, fd_set *writeSet, fd_set *rSet, fd_set *wSet)
{
	int yes = 1;
	std::string to_parse;
	std::string host;

	_readSet = readSet;
	_writeSet = writeSet;
	_wSet = wSet;
	_rSet = rSet;

	to_parse = _conf[0]["server|"]["listen"];
	errno = 0;
	if ((_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw (ServerException("socket()", std::string(strerror(errno))));
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw (ServerException("setsockopt()", std::string(strerror(errno))));
	if (to_parse.find(":") != std::string::npos)
	{
		host = to_parse.substr(0, to_parse.find(":"));
		if ((_port = atoi(to_parse.substr(to_parse.find(":") + 1).c_str())) < 0)
			throw (ServerException("Wrong port", std::to_string(_port)));
		_info.sin_addr.s_addr = inet_addr(host.c_str());
		_info.sin_port = htons(_port);
	} else
	{
		_info.sin_addr.s_addr = INADDR_ANY;
		if ((_port = atoi(to_parse.c_str())) < 0)
			throw (ServerException("Wrong port", std::to_string(_port)));
		_info.sin_port = htons(_port);
	}
	_info.sin_family = AF_INET;
	if (bind(_fd, (struct sockaddr *) &_info, sizeof(_info)) == -1)
		throw (ServerException("bind()", std::string(strerror(errno))));
	if (listen(_fd, 256) == -1)
		throw (ServerException("listen()", std::string(strerror(errno))));
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw (ServerException("fcntl()", std::string(strerror(errno))));
	FD_SET(_fd, _rSet);
	_maxFd = _fd;
	utils::showMessage("[" + std::to_string(_port) + "] " + "listening...");
}

void Server::refuseConnection()
{
	int fd = -1;
	struct sockaddr_in info;
	socklen_t len;

	errno = 0;
	len = sizeof(struct sockaddr);
	if ((fd = accept(_fd, (struct sockaddr *) &info, &len)) == -1)
		throw (ServerException("accept()", std::string(strerror(errno))));
	if (_tmp_clients.size() < 10)
	{
		_tmp_clients.push(fd);
		FD_SET(fd, _wSet);
	} else
		close(fd);
}

void Server::acceptConnection()
{
	int fd = -1;
	struct sockaddr_in info = {};
	socklen_t len;
	Client *newOne = NULL;

	memset(&info, 0, sizeof(struct sockaddr));
	errno = 0;
	len = sizeof(struct sockaddr);
	if ((fd = accept(_fd, (struct sockaddr *) &info, &len)) == -1)
		throw (ServerException("accept()", std::string(strerror(errno))));
	if (fd > _maxFd)
		_maxFd = fd;
	newOne = new Client(fd, _rSet, _wSet, info);
	_clients.push_back(newOne);
	utils::showMessage("[" + std::to_string(_port) + "] " + "connected clients: " + std::to_string(_clients.size()));
}

int Server::readRequest(std::vector<Client *>::iterator it)
{
	int bytes;
	int ret;
	Client *client = NULL;
	std::string log;

	client = *it;
	bytes = strlen(client->buffer);
	ret = read(client->fd, client->buffer + bytes, BUFFER_SIZE - bytes);
	bytes += ret;
	if (ret > 0)
	{
		client->buffer[bytes] = '\0';
		if (strstr(client->buffer, "\r\n\r\n") != NULL
			&& client->status != BODYPARSING)
		{
			log = "REQUEST:\n";
			log += client->buffer;
			utils::showMessage(log);
			client->lastDate = ft::getDate();
			_handler.parseRequest(*client, _conf);
			client->setWriteState(true);
		}



		if (client->status == BODYPARSING)
			_handler.parseBody(*client);
		return (1);
	} else
	{
		*it = NULL;
		_clients.erase(it);
		if (client)
			delete client;
		utils::showMessage("[" + std::to_string(_port) + "] " + "connected clients: " + std::to_string(_clients.size()));
		return (0);
	}
}

int Server::writeResponse(std::vector<Client *>::iterator it)
{
	unsigned long bytes;
	std::string tmp;
	std::string log;
	Client *client = NULL;

	client = *it;
	switch (client->status)
	{
		case RESPONSE:
			log = "RESPONSE:\n";
			log += client->textResponse.substr(0, 128);
			utils::showMessage(log);
			bytes = write(client->fd, client->textResponse.c_str(), client->textResponse.size());
			if (bytes < client->textResponse.size())
				client->textResponse = client->textResponse.substr(bytes);
			else
			{
				client->textResponse.clear();
				client->setToStandBy();
			}
			client->lastDate = ft::getDate();
			break;
		case STANDBY:
			if (getTimeDiff(client->lastDate) >= TIMEOUT)
				client->status = DONE;
			break;
		case DONE:
			delete client;
			_clients.erase(it);
			utils::showMessage("[" + std::to_string(_port) + "] " + "connected clients: " + std::to_string(_clients.size()));
			return (0);
		default:
			_handler.dispatcher(*client);
	}
	return (1);
}

void Server::send503(int fd)
{
	Response response;
	std::string str;
	int ret = 0;

	response.version = "HTTP/1.1";
	response.statusCode = UNAVAILABLE;
	response.headers["Retry-After"] = RETRY;
	response.headers["Date"] = ft::getDate();
	response.headers["Server"] = "webserv";
	response.body = UNAVAILABLE;
	response.headers["Content-Length"] = std::to_string(response.body.size());
	std::map<std::string, std::string>::const_iterator b = response.headers.begin();
	str = response.version + " " + response.statusCode + "\r\n";
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
		FD_CLR(fd, _wSet);
		_tmp_clients.pop();
	}
	utils::showMessage("[" + std::to_string(_port) + "] " + "connection refused, sent 503");
}


int Server::getTimeDiff(std::string start)
{
	struct tm start_tm;
	struct tm *now_tm;
	struct timeval time;
	int result;

	strptime(start.c_str(), "%a, %d %b %Y %T", &start_tm);
	gettimeofday(&time, NULL);
	now_tm = localtime(&time.tv_sec);
	result = (now_tm->tm_hour - start_tm.tm_hour) * 3600;
	result += (now_tm->tm_min - start_tm.tm_min) * 60;
	result += (now_tm->tm_sec - start_tm.tm_sec);
	return (result);
}

Server::ServerException::ServerException(void)
{
	this->error = "Undefined Server Exception";
}

Server::ServerException::ServerException(std::string function, std::string error)
{
	this->error = function + ": " + error;
}

Server::ServerException::~ServerException(void) throw()
{}

const char *Server::ServerException::what(void) const throw()
{
	return (this->error.c_str());
}
