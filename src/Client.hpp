//
// Created by Qsymond on 12.02.2021.
//

#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP
#include "Utils.hpp"

class Client
{
private:
	int			clientSocket;	//	Номер сокета клиента
	sockaddr_in	clientAddress;	//	Адрес клиента
public:
	Client(int newSock, sockaddr_in const &sockaddrIn);
	virtual ~Client();

	int getClientSocket() const;

	const sockaddr_in &getClientAddress() const;
};


#endif //WEBSERV_CLIENT_HPP
