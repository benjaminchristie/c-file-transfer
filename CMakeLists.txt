cmake_minimum_required(VERSION 3.10)
project(simple-socket)

set(CMAKE_C_FLAGS "-O3")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb3")
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_executable(server
	./src/server.c
)
target_include_directories(server PRIVATE ./include)
target_link_libraries(server sodium)

add_executable(client
	./src/client.c
)
target_include_directories(client PRIVATE ./include)
target_link_libraries(client sodium)
