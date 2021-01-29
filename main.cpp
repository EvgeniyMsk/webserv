#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <csignal>
#define port	80
int mySocket;
int client_socket;
int result;
void signalHandler( int signum ) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";
//	close(mySocket);
//	close(client_socket);
//	close(result);
	exit(signum);
}

int main() {
	struct sockaddr_in stSockAddr = {};
	mySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&stSockAddr, 0, sizeof (stSockAddr));
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons( port );
	stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	result = bind(mySocket, ( struct sockaddr* )&stSockAddr, sizeof( struct sockaddr ) );
	if (result == -1)
	{
		perror("Ошибка: связывания");
		close(mySocket);
		exit(EXIT_FAILURE);
	}
	if (listen(mySocket, 10) == -1)
	{
		perror("Ошибка: прослушивания");
		close(mySocket);
		exit(EXIT_FAILURE);
	}
	for (;;)
	{
		client_socket = accept(mySocket, nullptr, nullptr);
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
			response << "HTTP/1.1 200 OK\r\n"
					 << "Version: HTTP/1.1\r\n"
					 << "Content-Type: text/html; charset=utf-8\r\n"
					 << "Content-Length: " << response_body.str().length()
					 << "\r\n\r\n"
					 << response_body.str();

			// Отправляем ответ клиенту с помощью функции send
			result = send(client_socket, response.str().c_str(),
						  response.str().length(), 0);
			if (result < 0)
			{
				// произошла ошибка при отправле данных
				std::cout << "send failed: " << "\n";
			}
			// Закрываем соединение к клиентом
			close(client_socket);
		}
		signal(SIGINT, signalHandler);
	}
}