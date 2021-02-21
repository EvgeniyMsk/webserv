//
// Created by Qsymond on 18.02.2021.
//

#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP
#include <string>

class Request
{
public:
	int clientSocket;
	std::string response;
};


#endif //WEBSERV_REQUEST_HPP
