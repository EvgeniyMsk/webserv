#include "Webserv.hpp"
#include <iostream>

int main(int argc, char **argv) {

	if (argc != 2)
	{
		std::cout << "Пример запуска: ./webserv путь_к_файлу_конфигурации\n";
		return 0;
	}
	Webserv webserv(argv[1]);
	try
	{
		webserv.startServer();
	}
	catch (std::runtime_error &e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
