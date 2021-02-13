#include "src/Webserv.hpp"

int main(int argv, char **args, char **env)
{
	Webserv webserv;
	webserv.run();
	return 0;
}

//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include "fcntl.h"
//#include <iostream>
//
//
//void error(std::string const &err)
//{
//	std::cout << &err << " error!" << std::endl;
//	exit(1);
//}
//
//int main(int argc, char **argv)
//{
//
//	int fd[2];
//	int result;
//
//	char string[] = "Hello, world!";
//	if (pipe(fd) < 0)
//		error("pipe()");
//	result = fork();
//	if (result < 0)
//		error("fork()");
//	else if (result > 0)
//	{
//		// Процесс - родитель
//		close(fd[0]);
//		write(fd[1], "Hello, world!", 14);
//		close(fd[1]);
//		printf("Parent exit\n");
//	}
//	else
//	{
//		// Процесс - потомок
//		close(fd[1]);
//		char resstring[14];
//		read(fd[0], resstring, 14);
//		printf("%s\n",resstring);
//		printf("Child exit\n");
//	}
//	return 0;
//}
