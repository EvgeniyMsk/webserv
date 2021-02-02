//
// Created by Qsymond on 02.02.2021.
//

#include "Webserv.hpp"

Webserv::Webserv(const std::string &ipAddress, int port) : ipAddress(ipAddress),
														   port(port),
														   serverName
														   ("webserv_21")
{

}

Webserv::~Webserv()
{

}

const std::string &Webserv::getIpAddress() const
{
	return ipAddress;
}

void Webserv::setIpAddress(const std::string &ipAddress)
{
	Webserv::ipAddress = ipAddress;
}

int Webserv::getPort() const
{
	return port;
}

void Webserv::setPort(int port)
{
	Webserv::port = port;
}

std::string Webserv::getCurrentTime() const
{
	std::string currentTime;
	struct timeval tv;
	if (gettimeofday(&tv, NULL) == 0)
	{
		time_t t = tv.tv_sec;
		struct tm *tm =  localtime(&t);
		char time[80];
		if (strftime(time, sizeof(time), TIME_FORMAT, tm) != 0)
			currentTime = std::string(time);
	}
	return currentTime;
}

void Webserv::init()
{
	struct sockaddr_in stSockAddr = {};
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&stSockAddr, 0, sizeof(stSockAddr));
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(port);
	stSockAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	int result = bind(serverSocket, (struct sockaddr *) &stSockAddr,
				  sizeof(struct sockaddr));
	if (result == -1)
	{
		perror("Ошибка: связывания");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}
	if (listen(serverSocket, 10) == -1)
	{
		perror("Ошибка: прослушивания");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}
	int client_socket;
	for (;;)
	{
		client_socket = accept(serverSocket, nullptr, nullptr);
		if (client_socket < 0)
		{
			perror("Ошибка: принятия");
			close(client_socket);
			exit(EXIT_FAILURE);
		}

		const int max_client_buffer_size = 1024;
		char buf[max_client_buffer_size];
		result = recv(client_socket, buf, max_client_buffer_size, 0);
		std::stringstream response; // сюда будет записываться ответ клиенту
		std::stringstream response_body; // тело ответа
		if (result < 0)
		{
			// ошибка получения данных
			std::cout << "recv failed: " << result << "\n";
			close(client_socket);
		} else if (result == 0)
		{
			// соединение закрыто клиентом
			std::cout << "connection closed...\n";
		} else if (result > 0)
		{
			// Мы знаем фактический размер полученных данных, поэтому ставим метку конца строки
			// В буфере запроса.
			buf[result] = '\0';

			// Данные успешно получены
			// формируем тело ответа (HTML)
			response_body << "<title>Test C++ HTTP Server</title>\n"
						  << "<h1>Test page</h1>\n"
						  << "<p>This is body of the test page...</p>\n"
						  << "<h2>Request headers</h2>\n"
						  << "<pre>" << buf << "</pre>\n"
						  << "<em><small>Test C++ Http Server</small></em>\n";

			// Формируем весь ответ вместе с заголовками
			response << "HTTP/1.1 200 OK" 	<< END_CHARS
					 << "Version: HTTP/1.1" << END_CHARS
					 << Allow 				<< POST << END_CHARS
					 << Content_Language 	<< "ru" << END_CHARS
					 << Content_Length 		<< "500" << END_CHARS
					 << Content_Location	<< "http://" << ipAddress << END_CHARS
					 << Content_Type 		<< "text/html; charset=utf-8" << END_CHARS
					 << Date 				<< getCurrentTime() << END_CHARS
					 << Last_Modified		<< getCurrentTime() << END_CHARS
					 << Location			<< "http:://" << ipAddress << END_CHARS
					 << Retry_After			<< ""
					 << Server 				<< serverName << END_CHARS

					 << response_body.str().length()
					 << "\r\n\r\n"
					 << response_body.str();

			// Отправляем ответ клиенту с помощью функции send
			result = send(client_socket, response.str().c_str(),
						  response.str().length(), 0);
			if (result < 0)
			{
				// произошла ошибка при отправке данных
				std::cout << "send failed: " << "\n";
			}
			// Закрываем соединение к клиентом
			close(client_socket);
		}
//		signal(SIGINT, signalHandler);
	}
}

const Webserv::string &Webserv::getServerName() const
{
	return serverName;
}
