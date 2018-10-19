cmake_minimum_required(VERSION 3.8)
project(c4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/main.cpp
        )

add_executable(c4 ${SOURCE_FILES})