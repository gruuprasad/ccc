cmake_minimum_required(VERSION 3.8)
project(c4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/main.cpp
        )

add_executable(c4 ${SOURCE_FILES})

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${test/catch.hpp})

set(TEST_FILES
        test/test_main.cpp
        test/first_test.cpp
        )

add_executable(tests ${TEST_FILES})
target_link_libraries(tests Catch)
add_test(NAME tests COMMAND tests)
