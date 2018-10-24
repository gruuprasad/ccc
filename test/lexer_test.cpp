#include <iostream>
#include <fstream>
#include <sstream>
#include "catch.hpp"
#include "../src/lexer/fast_lexer.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "../src/lexer/lexer_exception.hpp"
#include <iterator>

TEST_CASE("Lexer Smoke test.") {
  auto token_list = FastLexer("{a+z-3*55aa case }}// }}\na a1 +++++ \"aa\"ee").lex();
}

TEST_CASE("Lexer Simple Operator tests.") {
  REQUIRE(FastLexer("+").lex().front().getType() == TokenType::PLUS);
  REQUIRE(FastLexer("-").lex().front().getType() == TokenType::MINUS);
  REQUIRE(FastLexer("++").lex().front().getType() == TokenType::PLUSPLUS);
  REQUIRE(FastLexer("--").lex().front().getType() == TokenType::MINUSMINUS);
}

TEST_CASE("Lexer keyword max munch test.") {
  auto token_list = FastLexer("automa").lex();
  Token &token = token_list.front();
  REQUIRE(token.getType() == TokenType::IDENTIFIER);
  REQUIRE(token.getExtra() == "automa");
}

TEST_CASE("Lexer keyword prio test.") {
  REQUIRE(FastLexer("auto").lex().front().getType() == TokenType::AUTO);
}

TEST_CASE("Fast Lexer number test.") {
  auto firstToken = FastLexer("123").lex().front();
  REQUIRE(firstToken.getType() == TokenType::NUMBER);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "123");
}

TEST_CASE("Fast Lexer number with extra test.") {
  auto token_list = FastLexer("123afg").lex();
  auto firstToken = token_list.front();
  auto secondToken = token_list.back();
  REQUIRE(firstToken.getType() == TokenType::NUMBER);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "123");
  REQUIRE(secondToken.getType() == TokenType::IDENTIFIER);
  REQUIRE(secondToken.getLine() == 1);
  REQUIRE(secondToken.getColumn() == 4);
  REQUIRE(secondToken.getExtra() == "afg");
}

TEST_CASE("Fast Lexer character constant test.") {
  {
    auto tokenList = FastLexer("'a'").lex();
    auto & firstToken = tokenList.front();
    REQUIRE(firstToken.getType() == TokenType::CHARACTER);
    REQUIRE(firstToken.getLine() == 1);
    REQUIRE(firstToken.getColumn() == 1);
    REQUIRE(firstToken.getExtra() == "a");
  }
  {
    auto tokenList = FastLexer("'\\r'").lex();
    auto & firstToken = tokenList.front();
    REQUIRE(firstToken.getType() == TokenType::CHARACTER);
    REQUIRE(firstToken.getLine() == 1);
    REQUIRE(firstToken.getColumn() == 1);
    REQUIRE(firstToken.getExtra() == "\\r");
  }
}

TEST_CASE("Fast Lexer invalid character literal test.") {
  REQUIRE_THROWS_AS(FastLexer("''").lex(), LexerException);
}

TEST_CASE("Fast Lexer line comment test.") {
  auto token_list = FastLexer("  aaa//blah\ntest//hehe\r\nmore//test\rtesting").lex();
  auto lastToken = token_list.back();
  REQUIRE(lastToken.getType() == TokenType::IDENTIFIER);
  REQUIRE(lastToken.getLine() == 4);
  REQUIRE(lastToken.getColumn() == 1);
  REQUIRE(lastToken.getExtra() == "testing");
}

TEST_CASE("Fast Lexer block comment test.") {
  auto token_list = FastLexer(" /**/x").lex();
  auto lastToken = token_list.back();
  REQUIRE(lastToken.getType() == TokenType::IDENTIFIER);
  REQUIRE(lastToken.getLine() == 1);
  REQUIRE(lastToken.getColumn() == 6);
  REQUIRE(lastToken.getExtra() == "x");
}

TEST_CASE("Fast Lexer block comment multiline test.") {
  auto token_list = FastLexer(" /*\nee*/x").lex();
  auto lastToken = token_list.back();
  REQUIRE(lastToken.getType() == TokenType::IDENTIFIER);
  REQUIRE(lastToken.getLine() == 2);
  REQUIRE(lastToken.getColumn() == 5);
  REQUIRE(lastToken.getExtra() == "x");
}

TEST_CASE("Fast Lexer block comment multiline unterminated.") {
  REQUIRE_THROWS_WITH(FastLexer(" /*\nee/x").lex(), "1:2: error: 'Unterminated Comment!'. Lexing Stopped!");
}

TEST_CASE("Fast Lexer string literals test.") {
  std::string input = "\"strings are slow\"";
  auto tokenList = FastLexer(input).lex();
  auto &firstToken = tokenList.front();
  REQUIRE(firstToken.getType() == TokenType::STRING);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "strings are slow");
}

TEST_CASE("Fast Lexer string escape sequence test.") {
  auto tokenList = FastLexer("\"strings \\n are slow\"").lex();
  auto &firstToken = tokenList.front();
  REQUIRE(firstToken.getType() == TokenType::STRING);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "strings \\n are slow");
}

TEST_CASE("Fast Lexer invalid string literal test.") {
  REQUIRE_THROWS_AS(FastLexer("\"this has invalid escape \\z\"").lex(), LexerException);
}
