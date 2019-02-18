cmake_minimum_required(VERSION 2.8.9)
project(websocket_server)

include_directories(include)


file(GLOB SOURCES "src/*.cpp")
 
add_library(websocket_server STATIC ${SOURCES})
find_package(Boost REQUIRED)
target_include_directories(websocket_server  PRIVATE ${Boost_INCLUDE_DIRS})
install(TARGETS websocket_server DESTINATION lib)