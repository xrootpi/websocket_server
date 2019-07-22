cmake_minimum_required(VERSION 2.8.9)
project(websocket-server)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_library(websocket-server STATIC ${SOURCES})

find_package(Boost REQUIRED)
find_package(Async-tcp REQUIRED)
target_include_directories(websocket-server PRIVATE ${Boost_INCLUDE_DIRS})

install(TARGETS websocket-server DESTINATION lib)
