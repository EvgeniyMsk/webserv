#include "Server.hpp"
#include "Web.hpp"
#include "CGI.hpp"
#include "Position.hpp"
#include <signal.h>
#include <string>
#include <iostream>
#include <vector>

Web web;

void handler(int signal)
{
	(void)signal;
	web.stop();
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    std::string path_to_config;
    if  (argc == 1)
        path_to_config = CONFIG_FILE_DEFAULT_PATH;
    else if (argc == 2)
         path_to_config = argv[1];
    else
    {
        std::cout << "Provide only one argument" << std::endl;
        exit(EXIT_FAILURE);
    }
	signal(SIGINT, handler);
	web = Web(path_to_config);
	web.serveConnections();
    // CGI cgi;
    // cgi.run();
    // не получается нормально подключить cgi, заглавная функция в cgi run(), скорее всего нужно ее твоему Server handleData() подключить, у меня сегодня так и не вышло
    // завтра попробую еще
    return (0);
}

// #include <stdio.h>
// #include <sys/time.h>
// #include <sys/types.h>
// #include <unistd.h>
// int 
// main(void) {
//     fd_set rfds;
//     struct timeval tv;
//     int retval;
//     /* Ждем, пока на стандартном вводе (fd 0) что-нибудь
//        появится. */
//     FD_ZERO(&rfds);
//     FD_SET(0, &rfds);
//     /* Ждем не больше пяти секунд. */
//     tv.tv_sec = 5;
//     tv.tv_usec = 0;
//     retval = select(1, &rfds, NULL, NULL, &tv);
//     /* Не полагаемся на значение tv! */
//     if (retval)
//         printf("Данные доступны.\n");
//         /* Теперь FD_ISSET(0, &rfds) вернет истинное значение. */
//     else
//         printf("Данные не появились в течение пяти секунд.\n");
//     return 0;
// }