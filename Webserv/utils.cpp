//
// Created by Qsymond on 20.02.2021.
//

#include "Webserv.hpp"

Webserv::~Webserv()
{

}

bool Webserv::isServerSocket(int fd)
{
	for (int i = 0; i < serverSize; i++)
		if (fd == sockets[i])
			return (true);
	return (false);
}

void Webserv::clean()
{
	for (int i = 0; i <= maxFD; ++i)
	{
		if (FD_ISSET(i, &m_read_set))
			close(i);
	}
	exit(SIGQUIT);
}

void Webserv::doCGI(int clientSocket)
{
	int fd[2];
	int child;
	int status;

	status = pipe(fd);
	if (status < 0)
		std::cout << "pipe()" << std::endl;
	child = fork();
	if (child < 0)
		std::cout << "fork()" << std::endl;
	else if (child == 0)
	{
		// Процесс - потомок
		close(fd[0]);
		dup2(fd[1], 1);
		execve("../cgi-bin/main", nullptr,nullptr);
		close(fd[1]);
		printf("Child exit\n");
	}
	else
	{
		// Процесс - родитель
		close(fd[1]);
		char resstring[200];
		read(fd[0], resstring, 200);
		close(fd[0]);
		std::stringstream response;
		std::stringstream response_body;
		response_body << resstring;
		// Формируем весь ответ вместе с заголовками
		response 	<< "HTTP/1.1 200 OK\r\n"
					<< "Version: HTTP/1.1\r\n"
					<< "Content-Type: text/html; charset=utf-8\r\n"
					<< "Content-Length: " << response_body.str().length()
					<< "\r\n\r\n"
					<< response_body.str();
		int result = send(clientSocket, response.str().c_str(), response.str().length(), 0);
		waitpid(child, &status, 0);
		if (WIFEXITED(status))
		{
			int exit_code = WEXITSTATUS(status);
			if (exit_code == 2)
				utils::exitWithLog();
		}
		if (result < 0)
		{
			m_close = true;
			utils::exitWithLog();
		}
	}
}
