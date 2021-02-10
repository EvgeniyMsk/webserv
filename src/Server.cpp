//
// Created by Qsymond on 10.02.2021.
//

#include "Server.hpp"

Server::Server(int newPort, std::string const &serverName) : port(newPort), serverName(serverName), BUFFER_SIZE(10000)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
		utils::exitWithLog();

	int reuse = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
		utils::exitWithLog();

	setNonBlock(serverSocket);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		utils::exitWithLog();

	if (listen(serverSocket, SOMAXCONN) < 0)
		utils::exitWithLog();
}

const int &Server::getServerSocket() const
{
	return (serverSocket);
}

void Server::acceptConnection()
{
	sockaddr_in newAddr;
	socklen_t size = sizeof(newAddr);
	int sock = accept(serverSocket, reinterpret_cast<sockaddr *>(&newAddr), &size);
	if(sock < 0)
		utils::exitWithLog();
	setNonBlock(sock);
	clientSockets.push_back(sock);
	std::cout << "New connection to " << serverName << " from " <<  inet_ntoa(newAddr.sin_addr) << ":" << utils::getSwapped(newAddr.sin_port) <<
	std::endl;
}

void Server::readRequest(fd_set *globalReadSetPtr)
{
	char buf[1024];
	int bytes_read;
	std::list<int>::iterator it = clientSockets.begin();
	std::string data;
	while (it != clientSockets.end())
	{
		if (FD_ISSET(*it, globalReadSetPtr))
		{
			// Поступили данные от клиента, читаем их
			bytes_read = receiveData(*it, data);
			if (bytes_read <= 0)
			{
				// Соединение разорвано, удаляем сокет из множества
				std::cout << "Client " << *it << " disconnected" << std::endl;
				close(*it);
				it = clientSockets.erase(it);
				continue;
			} else if (bytes_read > 0)
					std::cout << data;

			std::stringstream response;
			std::stringstream response_body;
			response_body << "<title>Test C++ HTTP Server</title>\n"
						  << "<h1>Test page for " << serverName << "</h1>\n";

			// Формируем весь ответ вместе с заголовками
			response << "HTTP/1.1 200 OK\r\n"
					 << "Version: HTTP/1.1\r\n"
					 << "Content-Type: text/html; charset=utf-8\r\n"
					 << "Content-Length: " << response_body.str().length()
					 << "\r\n\r\n"
					 << response_body.str();
			int result = send((*it), response.str().c_str(), response.str().length(), 0);

//			std::string tmp = buf;
//			Request *tmpRequest = new Request(tmp);
//			clientRequest[*it] = tmpRequest;
//			it = clientSockets.erase(it);
//			close(*it);
		} else
			it++;
	}
}

const int &Server::getMaxFd() const
{
	return (maxFd);
}

void Server::updateMaxFd()
{
	int maxTmp = serverSocket;
	if (!clientSockets.empty())
		maxTmp = std::max(maxTmp, *std::max_element(clientSockets.begin(), clientSockets.end()));
	maxFd = maxTmp;
}

std::list<int> &Server::getReadClients()
{
	return (clientSockets);
}

void Server::setNonBlock(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		utils::exitWithLog();
}

int   Server::receiveData(int client_sock, std::string &str)
{
	char buffer[BUFFER_SIZE + 1];

	int len = recv(client_sock, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (len <= 0)
		return (len);
	buffer[len] = '\0';
	str = buffer;
	return (1);
}

const sockaddr_in &Server::getServerAddress() const
{
	return serverAddress;
}

int Server::getPort() const
{
	return port;
}

const std::string &Server::getServerName() const
{
	return serverName;
}
