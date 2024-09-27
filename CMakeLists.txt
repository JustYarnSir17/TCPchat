cmake_minimum_required(VERSION 3.10)
project(TCPchat)

#서버 및 클라이언트 하위 프로젝트 포함하기
add_subdirectory(TCP)
add_subdirectory(TCP_cli)

#실행 커맨드를 추가해서 서버와 클라이언트를 동시에 실행한다.
add_custom_target(run_all ALL
	COMMAND $<TARGET_FILE:server> 
	COMMAND $<TARGET_FILE:client> 
	WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
	COMMENT "Running server and client..."
	DEPENDS server client
)