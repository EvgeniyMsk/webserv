#include "Client.hpp"
#include "Server.hpp"
#include <cerrno>

int fuckingSIGPIPE;

Client::Client(Server* S) : server(S), clientSocket(), port(), isConnected(true), addr(), size(sizeof(addr)), lastRequest(0), request()
{
	bzero(&addr, size);
	this->clientSocket = accept(S->getSocketFd(), (struct sockaddr *) &addr, &size);
	if (this->clientSocket == -1)
	{
		std::cerr << "Ошибка accept()\n";
		throw std::runtime_error(strerror(errno));
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Ошибка при установке неблокируемого соединения\n";
		throw std::runtime_error(strerror(errno));
	}
	int opt = 1;
	if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Ошибка установки опций сокета\n";
		throw std::runtime_error(strerror(errno));
	}
	this->host = inet_ntoa(addr.sin_addr);
	this->port = htons(addr.sin_port);
	this->ipAddress = host + ':' + ft::inttostring(port);
	if (CONNECTION_LOGS)
		std::cerr << "Opened a new client for " << clientSocket << " at " << ipAddress << std::endl;
}

Client::~Client()
{
	close(clientSocket);
	clientSocket = -1;
	req.clear();
	this->request.clear();
}

int Client::receiveRequest()
{
	char buf[BUFFERSIZE + 1];
	int recvRet;
	bool recvCheck(false);

	memset(buf, 0, BUFFERSIZE);
	this->resetTimeout();
	while ((recvRet = recv(this->clientSocket, buf, BUFFERSIZE, 0)) > 0)
	{
		buf[recvRet] = '\0';
		this->req.append(buf);
		recvCheck = true;
	}
	if (!recvCheck or recvRet == 0)
	{
		this->isConnected = false;
		return (0);
	}
	return (1);
}

void Client::sendReply(const char* msg, request_s& request) const
{
	long bytesToSend = strlen(msg),
			bytesSent(0),
			sendRet;

	fuckingSIGPIPE = 0;
	while (bytesToSend > 0)
	{
		sendRet = send(this->clientSocket, msg + bytesSent, bytesToSend, 0);
		if (sendRet == -1)
		{
			if (fuckingSIGPIPE == 0 && bytesToSend != 0)
				continue;
			throw std::runtime_error(strerror(errno));
		}
		bytesSent += sendRet;
		bytesToSend -= sendRet;
	}
	static int i = 1, post = 1;
	std::cerr << "sent response for req #" << i++ << " (" << ft::methodToString(request.method);
	if (request.method == POST)
		std::cerr << " #" << post++;
	std::cerr << ").\n";
}

void Client::resetTimeout()
{
	this->lastRequest = ft::getTime();
}

void Client::checkTimeout()
{
	if (this->lastRequest)
	{
		time_t diff = ft::getTime() - this->lastRequest;
		if (diff > 10000000)
			this->isConnected = false;
	}
}

void Client::reset(const std::string& connection)
{
	if (connection == "close")
	{
		if (CONNECTION_LOGS)
			std::cerr << "We ain't resetting, we're closing this client, baby" << std::endl;
		this->isConnected = false;
		return;
	} else if (!this->isConnected)
		return;
	if (CONNECTION_LOGS)
		std::cerr << "Resetting client!\n";
	this->isConnected = true;
	req.clear();
	lastRequest = 0;
	this->request.clear();
}

Client::Client() : server(), clientSocket(), port(), isConnected(), addr(), size(), lastRequest()
{

}

void Client::breakOnSIGPIPE(int)
{
	std::cerr << "Ошибка отправки ответа. Закрываю соединение.\n";
	fuckingSIGPIPE = 1;
}
