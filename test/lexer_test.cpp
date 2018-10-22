#include <iostream>
#include "catch.hpp"
#include "../src/lexer/lexer.hpp"

TEST_CASE("Lexer Smoke test.") {
  auto token_list = Lexer().lex("{a+z-3*55aa case }}// }}\na a1 +++++ \"aa\"ee");
  for (const auto &token : token_list) {
    std::cout << token << std::endl;
  }
}

TEST_CASE("Lexer Simple Operator tests.") {
  REQUIRE(Lexer().lex("+").front().getType() == TokenType::PLUS);
  REQUIRE(Lexer().lex("-").front().getType() == TokenType::MINUS);
  REQUIRE(Lexer().lex("++").front().getType() == TokenType::PLUSPLUS);
  REQUIRE(Lexer().lex("--").front().getType() == TokenType::MINUSMINUS);
}

TEST_CASE("Lexer keyword max munch test.") {
  auto token_list = Lexer().lex("automa");
  Token &token = token_list.front();
  REQUIRE(token.getType() == TokenType::IDENTIFIER);
  REQUIRE(token.getExtra() == "automa");
}

TEST_CASE("Lexer keyword prio test.") {
  REQUIRE(Lexer().lex("auto").front().getType() == TokenType::AUTO);
}
