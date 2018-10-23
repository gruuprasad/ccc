#include "catch.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "../src/lexer/reflex_lexer.hpp"

//TEST_CASE("Our compiler should fail for now.") {
//  REQUIRE(1 == EntryPointHandler().handle(0, nullptr));
//}
//
//TEST_CASE("Our tokenizer should work.") {
//  char *args[] = {
//      (char*)"c4",
//      (char*)"--tokenize",
//      (char*)"../examples/test.c",
//      nullptr
//  };
//  REQUIRE(0 == EntryPointHandler().handle(3, args));
//}
//
TEST_CASE("Our tokenizer should fail on invalid.") {
  char *args[] = {
      (char*)"c4",
      (char*)"--tokenize",
      (char*)"../examples/error.c",
      nullptr
  };
  REQUIRE(1 == EntryPointHandler().handle(3, args));
}

TEST_CASE("Smoke test generated lexer.") {
  FILE *fd = stdin;
  if ((fd = fopen("../examples/test.c", "r")) == nullptr) {
    std::cerr << "file not found!\r";
    return;
  }
  // create a lexer that consumes a file or reads stdin
  ReflexLexer lexer(fd);
  // here we go!
  lexer.lex();
  // display the results

  auto token_list = lexer.results();
  if (fd != stdin)
    fclose(fd);
}
