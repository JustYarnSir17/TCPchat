cmake_minimum_required(VERSION 3.10)
project(TCP)

set(CMAKE_CXX_STANDARD 11)

set(SERVER_SRC TCP/server.cpp)

add_executable(server ${SERVER_SRC})

if(WIN32)
	target_link_libraries(server ws2_32)
endif()