cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv srcs/main.cpp srcs/Client.cpp srcs/Client.hpp srcs/Webserv.cpp srcs/Webserv.hpp srcs/HTTP.cpp srcs/HTTP.hpp srcs/Server.cpp srcs/Server.hpp srcs/Utils.cpp srcs/Utils.hpp)