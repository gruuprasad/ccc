#include <iostream>
#include "catch.hpp"
#include "../src/lexer/lexer.hpp"

TEST_CASE("Lexer Smoke test.") {
  auto token_list = Lexer().lex("{a+z-3*55aa }} }}\na a1 aa");
  for (const auto &token : token_list){
    std::cout << token << std::endl;
  }
}
