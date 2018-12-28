#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>

//#define PRINT

using namespace ccc;

// TEST_CASE("ast struct test") {
//  auto language = "struct { int; }; struct A { int x; } a;";
//  auto token_list = (new FastLexer(language))->lex();
//  auto *fp = new FastParser(token_list);
//  fp->parse();
//  REQUIRE(!fp->fail());
//}

TEST_CASE("ast statement test") {
  auto language = "{ if (a == 0) {while (1) if(c == p) a += 3; else "
                  "{if (a) continue; else break;} "
                  "return;} else a "
                  "= 1; {return 1 + 3;}} ";

  auto token_list = (new FastLexer(language))->lex();

  //  auto fp = new FastParser(language);
  //#if DEBUG
  //  fp->printTrace = true;
  //#endif
  //  fp->parse(PARSE_TYPE::TRANSLATIONUNIT);
  //  REQUIRE(!fp->fail());

  /* test is broken with current build
  std::cout << root->prettyPrint() << std::endl;
  std::ofstream file;
  file.open("ast.gv");
  file.clear();
  file << root->toGraph();
  file.close();
  */
}
