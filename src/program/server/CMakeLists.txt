cmake_minimum_required(VERSION 3.16)
project(TcpServer)


set(CMAKE_CXX_STANDARD 20)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

add_executable(${PROJECT_NAME} TcpServer.cpp)