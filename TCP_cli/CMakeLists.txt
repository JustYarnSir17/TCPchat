cmake_minimum_required(VERSION 3.10)
project(TCP_cli)

set(CMAKE_CXX_STANDARD 11)

set(CLIENT_SRC TCP_cli/client.cpp)

add_executable(client ${CLIENT_SRC})

if(WIN32)
	target_link_libraries(client ws2_32)
endif()