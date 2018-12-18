add_executable(test_lexer
        test_main.cpp
        lexer_test.cpp
        lexer_punctuation_test.cpp
        lexer_keyword_test.cpp
        lexer_big_test.cpp
        first_test.cpp
        )

add_executable(test_parser
        test_main.cpp
        parser_test.cpp
        parser_simple.cpp
        )

target_link_libraries(test_lexer entry lexer)
add_dependencies(check test_lexer)
add_test(NAME lexer COMMAND test_lexer)

target_link_libraries(test_parser entry lexer parser ast)
add_dependencies(check test_parser)
add_test(NAME parser COMMAND test_parsers)
