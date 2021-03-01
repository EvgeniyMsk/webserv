#include "Webserv.hpp"
#include <iostream>
#include <zconf.h>
#include "Response.hpp"
#include <signal.h>
#include <cerrno>
#include <algorithm>
#include "Utils.hpp"

Webserv::Webserv() : w_read_set(), w_write_set(), m_read_set(), m_write_set()
{
	FD_ZERO(&w_read_set);
	FD_ZERO(&w_write_set);
	FD_ZERO(&m_read_set);
	FD_ZERO(&m_write_set);
	configPath = nullptr;
}

Webserv::Webserv(char *confPath) : w_read_set(), w_write_set(), m_read_set(), m_write_set()
{
	FD_ZERO(&w_read_set);
	FD_ZERO(&w_write_set);
	FD_ZERO(&m_read_set);
	FD_ZERO(&m_write_set);
	configPath = confPath;
}

Webserv::~Webserv()
{
	this->stopServer();
}

Webserv::Webserv(const Webserv &obj) : w_read_set(), w_write_set(), m_read_set(), m_write_set(), configPath()
{
	*this = obj;
}

Webserv& Webserv::operator=(const Webserv &webserv)
{
	if (this != &webserv)
	{
		this->w_read_set = webserv.w_read_set;
		this->w_write_set = webserv.w_write_set;
		this->m_read_set = webserv.m_read_set;
		this->m_write_set = webserv.m_write_set;
		this->servers = webserv.servers;
		this->configPath = webserv.configPath;
		this->connections = webserv.connections;
	}
	return *this;
}

void Webserv::startListening()
{
	std::cout << "Ожидание соединений..." << std::endl;
	while (true)
	{
		w_read_set = m_read_set;
		w_write_set = m_write_set;

		if (select(this->getMaxFD(), &w_read_set, &w_write_set, 0, 0) == -1)
			throw std::runtime_error(strerror(errno));

		for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			Server *s = *it;
			if (FD_ISSET(s->getSocketFd(), &w_read_set))
			{
				int clientfd = s->addConnection();
				this->connections[clientfd] = clientfd;
				FD_SET(clientfd, &m_read_set);
			}
			std::vector<Client *>::iterator itc = s->_connections.begin();
			while (itc != s->_connections.end())
			{
				Client *c = *itc;
				if (FD_ISSET(c->clientSocket, &w_read_set))
				{
					if (c->receiveRequest() == 1 && checkIfEnded(c->req))
					{
						FD_SET(c->clientSocket, &m_write_set);
					}
				}
				if (FD_ISSET(c->clientSocket, &w_write_set))
				{
					Request requestParser;
					Response responseHandler;
					std::string response;

					c->request = requestParser.parseRequest(c->req);
					c->request.server = c->server;
					c->request.location = c->server->matchlocation(c->request.uri);

					try
					{
						response = responseHandler.handleRequest(c->request);
						c->sendReply(response.c_str(), c->request);
						response.clear();
						c->reset(responseHandler._header_vals[CONNECTION]);
					} catch (std::exception &e)
					{
						c->isConnected = false;
					}
					FD_CLR(c->clientSocket, &m_write_set);
				}
				c->checkTimeout();
				if (!c->isConnected)
				{
					FD_CLR(c->clientSocket, &m_read_set);
					FD_CLR(c->clientSocket, &m_write_set);
					this->connections.erase(c->clientSocket);
					delete *itc;
					s->_connections.erase(itc);
					if (s->_connections.empty())
						break;
					else
					{
						itc = s->_connections.begin();
						continue;
					}
				}
				++itc;
			}
		}
	}
}

Webserv*	THIS;
void Webserv::startServer() {
	loadConfiguration();
	THIS = this;
	signal(SIGINT, Webserv::signalServer);
	signal(SIGPIPE, Client::breakOnSIGPIPE);
	startListening();
}

void Webserv::signalServer(int n) {
	std::cerr << "\nОшибка! Сервер остановлен!\n";
	THIS->stopServer();
	exit(n);
}

void Webserv::stopServer()
{
	std::vector<Server *>::iterator sit;
	for (sit = servers.begin(); sit != servers.end(); ++sit)
	{
		delete *sit;
	}
	servers.clear();
	FD_ZERO(&w_read_set);
	FD_ZERO(&w_write_set);
	FD_ZERO(&m_read_set);
	FD_ZERO(&m_write_set);
	std::cout << "Сервер успешно остановлен.\n";
}

std::vector<Server*>	parse(char *av)
{
	std::vector<Server *> skrtks;
	std::string str;
	struct stat statstruct = {};
	int fd;
	if (av && stat(av, &statstruct) != -1)
		fd = open(av, O_RDONLY);
	else
		fd = open("config/localhost.conf", O_RDONLY);
	if (fd < 0)
		return skrtks;

	while (ft::get_next_line(fd, str) > 0)
	{
		if (ft::is_first_char(str) || str.empty())
			continue;
		try
		{
			Server *tmp = new Server();
			if (!str.empty() && str.compare(str.find_first_not_of(" \t\n"), strlen("server {"), "server {") == 0)
			{
				tmp->setup(fd);
				skrtks.push_back(tmp);
			}
		}
		catch (std::exception &e)
		{
			std::cerr << "Ошибка при парсинге блока: " << e.what() << std::endl;
			close(fd);
			exit(EXIT_FAILURE);
		}
	}
	close(fd);
	return skrtks;
}

void Webserv::loadConfiguration()
{
	FD_SET(0, &m_read_set);
	this->servers = parse(configPath);
	for (std::vector<Server *>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::cout << *(*it);
		(*it)->startListening();
		FD_SET((*it)->getSocketFd(), &m_read_set);
		this->connections[(*it)->getSocketFd()] = (*it)->getSocketFd();
	}
}

bool Webserv::checkIfEnded(const std::string& request)
{
	size_t chunkedPos;
	size_t encoPos = request.find("Transfer-Encoding:");
	if (encoPos != std::string::npos)
		chunkedPos = request.find("chunked", encoPos);
	if (encoPos != std::string::npos && chunkedPos != std::string::npos)
	{
		size_t endSequencePos = request.find("\r\n0\r\n\r\n");
		size_t len = request.length();
		if (endSequencePos != std::string::npos && endSequencePos + 7 == len)
		{
			return true;
		}
	} else
	{
		size_t endSequencePos = request.find_last_not_of("\r\n\r\n");
		size_t len = request.length();
		if (endSequencePos != std::string::npos && endSequencePos + 5 == len)
		{
			return true;
		}
	}
	return false;
}

int Webserv::getMaxFD()
{
	return (*std::max_element(this->connections.begin(), this->connections.end())).second + 1;
}
