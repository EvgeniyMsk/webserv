cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv main.cpp headers/RequestHeaders.cpp headers/RequestHeaders.hpp headers/ResponseHeaders.cpp headers/ResponseHeaders.hpp headers/defines.hpp src/Webserv.cpp src/Webserv.hpp src/Config.cpp src/Config.hpp src/Server.cpp src/Server.hpp src/Utils.hpp src/Utils.cpp src/Request.hpp)