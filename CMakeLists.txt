cmake_minimum_required(VERSION 3.16)
project(miniRedis)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
include_directories(${CMAKE_SOURCE_DIR}/src/base)
include_directories(${CMAKE_SOURCE_DIR}/src/net)
include_directories(${CMAKE_SOURCE_DIR}/src/datastruct)
add_subdirectory(src)
add_subdirectory(test)
add_executable(miniRedis main.cpp)
