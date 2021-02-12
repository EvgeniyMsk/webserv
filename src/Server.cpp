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
	serverAddress.sin_addr.s_addr = inet_addr(LOCALHOST);
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
	sockaddr_in newAddr = {};
	socklen_t size = sizeof(newAddr);
	int sock = accept(serverSocket, reinterpret_cast<sockaddr *>(&newAddr), &size);
	if(sock < 0)
		utils::exitWithLog();
	setNonBlock(sock);
	clients.push_back(new Client(sock, newAddr));
	std::cout << "New connection to " << serverName << " from " <<  inet_ntoa(newAddr.sin_addr) << ":" << utils::getSwapped(newAddr.sin_port) <<
	std::endl;
}

void Server::readRequest(fd_set read_set)
{
	char buf[1024];
	int bytes_read;
	std::vector<Client *>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (FD_ISSET((*it)->getClientSocket(), &read_set))
		{
			std::string data;
			// Поступили данные от клиента, читаем их
			bytes_read = receiveData((*it)->getClientSocket(), data);
			if (bytes_read <= 0)
			{
				// Соединение разорвано, удаляем сокет из множества
				std::cout << "Client " << (*it)->getClientSocket() << " disconnected" << std::endl;
				close((*it)->getClientSocket());
				it = clients.erase(it);
				continue;
			} else if (bytes_read > 0)
			{
				std::cout << data;
				handleData(*it, data);
			}
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
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
		if ((*it)->getClientSocket() > maxTmp)
			maxTmp = (*it)->getClientSocket();
	maxFd = maxTmp;
}

std::vector<Client *> Server::getReadClients()
{
	return (clients);
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

void Server::writeResponse(fd_set write_set)
{

}

void Server::handleData(Client *client, const std::string &data)
{
	std::stringstream response;
	std::stringstream response_body;
	response_body << "<title>Test C++ HTTP Server</title>\n" << "<h1>Test page for " << serverName << "</h1>\n";
	// Формируем весь ответ вместе с заголовками
	response 	<< "HTTP/1.1 200 OK\r\n"
				<< "Version: HTTP/1.1\r\n"
				<< "Content-Type: text/html; charset=utf-8\r\n"
				<< "Content-Length: " << response_body.str().length()
				<< "\r\n\r\n"
				<< response_body.str();
	int result = send(client->getClientSocket(), response.str().c_str(), response.str().length(), 0);
}
