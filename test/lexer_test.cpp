#include "catch.hpp"
#include "../src/lexer/lexer.hpp"

TEST_CASE("Lexer Smoke test.") {
  REQUIRE(0 == Lexer().lex("{a+z-3*55aa }} }}\na a1 aa"));
}
