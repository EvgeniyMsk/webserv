cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv main.cpp Webserv/Webserv.cpp Webserv/Webserv.hpp Config/Config.cpp Config/Config.hpp Utils/Utils.cpp Utils/Utils.hpp Webserv/Client.cpp Webserv/Client.hpp Request/Request.cpp Request/Request.hpp cgi-bin/tester.cpp Webserv/initialize.cpp Webserv/utils.cpp)