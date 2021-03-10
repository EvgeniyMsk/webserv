#include "Webserv.hpp"

bool isServerRunning = true;

int main(int ac, char **av)
{
	Webserv webserv;
	if (ac != 2)
		std::cout << "Запуск сервера: ./webserv путь_к_конфигу\n";
	try
	{
		webserv.parse(av[1], webserv.servers);
		webserv.init();
		webserv.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Ошибка: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
