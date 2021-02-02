cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv main.cpp src/Webserv.cpp src/Webserv.hpp headers/RequestHeaders.cpp headers/RequestHeaders.hpp headers/ResponseHeaders.cpp headers/ResponseHeaders.hpp headers/defines.hpp)