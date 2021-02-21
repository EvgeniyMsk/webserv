//
// Created by Qsymond on 18.02.2021.
//

#include "Webserv.hpp"

bool isServerRunning = true;

void Webserv::send_data(int fd)
{

	std::stringstream response;
	std::stringstream response_body;
	// Формируем весь ответ вместе с заголовками
	response_body << "<title>Test C++ HTTP Server</title>\n"
				  << "<h1>Test page</h1>\n"
				  << "<p>This is body of the test page...</p>\n"
				  << "<h2>Request headers</h2>\n"
				  << "<em><small>Test C++ Http Server</small></em>\n";
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.str().length()
			 << "\r\n\r\n"
			 << response_body.str();
	if (int rc = send(fd, response.str().c_str(),response.str().length(), 0) < 0)
	{
		m_close = true;
		std::cout << "send() error\n";
		return;
	}
//	doCGI(fd);
}

void Webserv::close_conn(int fd)
{
	if (m_close) {
		FD_CLR(fd, &m_read_set);
		FD_CLR(fd, &m_write_set);
		close(fd);
		if (fd == maxFD) {
			while (!(FD_ISSET(maxFD, &m_read_set)) && !(FD_ISSET(maxFD, &m_write_set)))
				maxFD -= 1;
		}
	}
}


void Webserv::accept_connection(int fd)
{
	int new_sock = 0;
	struct sockaddr_in info;
	socklen_t len = sizeof(struct sockaddr);

	memset(&info, 0, sizeof(struct sockaddr));

	while (new_sock != -1)
	{
		new_sock = accept(fd, (struct sockaddr *) &info, &len);
		if (new_sock < 0)
		{
			if (errno != EWOULDBLOCK)
				strerror(errno);
			break;
		}
		set_non_block(new_sock);
		FD_SET(new_sock, &m_read_set);
		FD_SET(new_sock, &m_write_set);
		if (new_sock > maxFD)
			maxFD = new_sock;
	}
	std::cout << "New connection to fd: " << fd << std::endl;
}

void Webserv::receiveData(int client_sock)
{
	char buffer[10000];
	int len;
	int rc;

	rc = read(client_sock, buffer, sizeof(buffer));
	if (rc < 0)
	{
		m_close = true;
		return;
	}
	else if (rc == 0)
	{
		printf("  Connection closed\n");
		return;
	}
	else
	{
		buffer[len] = '\0';
		len = rc;
		printf("  %d bytes received\n", len);
		std::cout << buffer;
//		send_data(client_sock);
	}
}

void Webserv::run()
{
	int    changedSockets;


	while (isServerRunning)
	{

		memcpy(&w_read_set, &m_read_set, sizeof(m_read_set));
		memcpy(&w_write_set, &m_write_set, sizeof(m_write_set));

		printf("Waiting on select()...\n");
		changedSockets = select(maxFD + 1, &w_read_set, &w_write_set, NULL, NULL);
//		changedSockets = select(maxFD + 1, &w_read_set, nullptr, NULL, nullptr);
		if (changedSockets < 0)
		{
			strerror(errno);
		}

		for (int i=0; i <= maxFD  &&  changedSockets > 0; ++i)
		{
			if (FD_ISSET(i, &w_write_set))
			{
				send_data(i);
				std::cout << i << " in write set\n";
				close_conn(i);
			}
			else
				if (FD_ISSET(i, &w_read_set))
			{
				changedSockets -= 1;
				if (isServerSocket(i))
				{
					accept_connection(i);
				}
				else
				{
					m_close = false;
					receiveData(i);
					close_conn(i);
				}
			}
		}
	}
	clean();
}

void Webserv::exiting(int n)
{
	(void)n;
	std::cout << "\n" << "exiting...\n";
	isServerRunning = false;
}



