cmake_minimum_required(VERSION 2.8.9)
project(websocket_server)

include_directories(include)


file(GLOB SOURCES "src/*.cpp")
 
add_library(websocket_server STATIC ${SOURCES})

install(TARGETS websocket_server DESTINATION lib)