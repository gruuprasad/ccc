cmake_minimum_required(VERSION 3.12.2)
project(c4)

set(CMAKE_CXX_STANDARD 11)

if (ENABLE_COVERAGE_TESTING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage")
endif ()

set(SOURCE_FILES
        src/main.cpp
        src/entry/entry_point_handler.cpp src/entry/entry_point_handler.hpp
        src/lexer/lexer.cpp src/lexer/lexer.hpp
        src/lexer/token_type.cpp src/lexer/token_type.hpp
        src/lexer/token.cpp src/lexer/token.hpp
        src/lexer/reflex_lexer.cpp src/lexer/reflex_lexer.hpp
        src/lexer/lexer_exception.cpp src/lexer/lexer_exception.hpp
        src/lexer/fast_lexer.cpp src/lexer/fast_lexer.hpp)

set(REFLEX
        src/reflex/abslexer.h
        src/reflex/absmatcher.h
        src/reflex/bits.h
        src/reflex/boostmatcher.h
        src/reflex/convert.cpp src/reflex/convert.h
        src/reflex/debug.cpp src/reflex/debug.h
        src/reflex/error.cpp src/reflex/error.h
        src/reflex/flexlexer.h
        src/reflex/input.cpp src/reflex/input.h
        src/reflex/matcher.cpp src/reflex/matcher.h
        src/reflex/pattern.cpp src/reflex/pattern.h
        src/reflex/posix.cpp src/reflex/posix.h
        src/reflex/ranges.h
        src/reflex/setop.h
        src/reflex/stdmatcher.h
        src/reflex/timer.h
        src/reflex/traits.h
        src/reflex/unicode.cpp src/reflex/unicode.h
        src/reflex/utf8.cpp src/reflex/utf8.h
        )

add_executable(c4 ${SOURCE_FILES} ${REFLEX})

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${test/catch.hpp})

list(SUBLIST SOURCE_FILES 1 -1 TEST_SOURCE_FILES)

set(TEST_FILES
        test/test_main.cpp
        test/first_test.cpp
        test/lexer_test.cpp
        test/lexer_keyword_test.cpp
        test/lexer_punctuation_test.cpp
        test/lexer_big_test.cpp
        )

add_executable(tests ${TEST_SOURCE_FILES} ${REFLEX} ${TEST_FILES})
target_link_libraries(tests Catch)
add_test(NAME tests COMMAND tests)
