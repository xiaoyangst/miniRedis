cmake_minimum_required(VERSION 3.16)
project(Test)


set(CMAKE_CXX_STANDARD 20)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

aux_source_directory(${CMAKE_SOURCE_DIR}/src/base BASE)


add_executable(StickyPackageTest StickyPackageTest.cpp ${BASE})
target_link_libraries(StickyPackageTest pthread)