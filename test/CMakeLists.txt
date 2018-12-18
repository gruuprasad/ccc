add_executable(tests
  test_main.cpp
  lexer_test.cpp
  parser_test.cpp
  lexer_punctuation_test.cpp
  lexer_keyword_test.cpp
  lexer_big_test.cpp
  first_test.cpp
  )

target_link_libraries(tests entry lexer parser)
add_dependencies(check tests)
add_test(NAME test COMMAND tests)
