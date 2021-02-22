//
// Created by Qsymond on 18.02.2021.
//

#include "Client.hpp"

Client::Client(int newSock, const sockaddr_in &sockaddrIn) : clientSocket(newSock), clientAddress(sockaddrIn)
{
}

Client::~Client()
{
}

int Client::getClientSocket() const
{
	return clientSocket;
}

const sockaddr_in &Client::getClientAddress() const
{
	return clientAddress;
}