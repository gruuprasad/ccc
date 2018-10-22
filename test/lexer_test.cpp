#include <iostream>
#include <fstream>
#include <sstream>
#include "catch.hpp"
#include "../src/lexer/lexer.hpp"
#include "../src/entry/entry_point_handler.hpp"

bool lexing_of(const std::string &filename, const std::string &result) {
  std::stringstream buffer;
  EntryPointHandler().tokenize(std::ifstream("../examples/" + filename), filename, buffer);
  const auto content = buffer.str();
  if (content != result) {
    std::cerr << std::endl << "content of " << filename << " did not match expected, got:" << std::endl << ">>>"
              << std::endl << content << std::endl << "---" << std::endl << result << ">>>" << std::endl;
    return false;
  }
  return true;
}

std::string to_match(const std::string &filename) {
  std::ifstream t("../examples/" + filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

TEST_CASE("Compare test.c") {
  REQUIRE(lexing_of("test.c", to_match("test.txt")));
}

TEST_CASE("Compare small.c") {
  REQUIRE(lexing_of("small.c", to_match("small.txt")));
}

TEST_CASE("Compare hello_world.c") {
  REQUIRE(lexing_of("hello_world.c", to_match("hello_world.txt")));
}

TEST_CASE("Lexer Smoke test.") {
  auto token_list = Lexer().lex("{a+z-3*55aa case }}// }}\na a1 +++++ \"aa\"ee");
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

