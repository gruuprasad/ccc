#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"

//#define PRINT

using namespace ccc;

TEST_CASE("Fast Parser: Struct declaration test") {
  auto language = "struct { int; }; struct A { int x; } a;";
  auto token_list = (new FastLexer(language))->lex();
  auto *fp = new FastParser(token_list);
  fp->parse();
  REQUIRE(!fp->fail());
}
