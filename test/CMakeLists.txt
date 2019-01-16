add_library(test_LLIB OBJECT test_main.cpp)
target_link_libraries(test_LLIB entry lexer parser ast)

add_executable(test_lexer
        lexer/lexer_test.cpp
        lexer/lexer_punctuation_test.cpp
        lexer/lexer_keyword_test.cpp
        lexer/lexer_big_test.cpp
        lexer/first_test.cpp
        )
target_link_libraries(test_lexer test_LLIB)
add_dependencies(check test_lexer)
add_test(NAME lexer COMMAND test_lexer)

add_executable(test_parser
        parser/parse_declaration.cpp
        parser/parse_expression.cpp
        parser/parser_big_test.cpp
        )
target_link_libraries(test_parser test_LLIB)
add_dependencies(check test_parser)
add_test(NAME parser COMMAND test_parser)

add_executable(test_ast
        ast/ast_test.cpp
        )
target_link_libraries(test_ast test_LLIB)
add_dependencies(check test_ast)
add_test(NAME ast COMMAND test_ast)

add_executable(test_prettyPrinter
        prettyPrinter/prettyPrinter_test.cpp
        prettyPrinter/prettyPrinter_entry_test.cpp
        )
target_link_libraries(test_prettyPrinter test_LLIB)
add_dependencies(check test_prettyPrinter)
add_test(NAME prettyPrinter COMMAND test_prettyPrinter)

add_executable(test_semantic
        semantic/typeAnalysis_test.cpp
        semantic/nameAnalysis_test.cpp
        )
target_link_libraries(test_semantic test_LLIB)
add_dependencies(check test_semantic)
add_test(NAME semantic COMMAND test_semantic)
