cmake_minimum_required(VERSION 3.12.3)
project(c4)

set(CMAKE_CXX_STANDARD 11)

if (ENABLE_COVERAGE_TESTING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage")
endif ()

set(SOURCE_FILES
        src/main.cpp
        src/entry/entry_point_handler.cpp src/entry/entry_point_handler.hpp
        )

add_executable(c4 ${SOURCE_FILES})

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${test/catch.hpp})

list(SUBLIST SOURCE_FILES 1 -1 TEST_SOURCE_FILES)

set(TEST_FILES
        test/test_main.cpp
        test/first_test.cpp
        )

add_executable(tests ${TEST_SOURCE_FILES} ${TEST_FILES})
target_link_libraries(tests Catch)
add_test(NAME tests COMMAND tests)
