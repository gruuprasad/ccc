#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "lexer/fast_lexer.hpp"
#include <iostream>
#include <iterator>
#include <sstream>

using namespace ccc;
TEST_CASE("Lexer Smoke test.") {
  auto token_list =
      FastLexer("{a+z-3*55aa case }}// }}\na a1 +++++ \"aa\"ee").lex();
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
  auto firstToken = token_list[0];
  auto secondToken = token_list[1];
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
    auto &firstToken = tokenList.front();
    REQUIRE(firstToken.getType() == TokenType::CHARACTER);
    REQUIRE(firstToken.getLine() == 1);
    REQUIRE(firstToken.getColumn() == 1);
    REQUIRE(firstToken.getExtra() == "a");
  }
  {
    auto tokenList = FastLexer("'\\r'").lex();
    auto &firstToken = tokenList.front();
    REQUIRE(firstToken.getType() == TokenType::CHARACTER);
    REQUIRE(firstToken.getLine() == 1);
    REQUIRE(firstToken.getColumn() == 1);
    REQUIRE(firstToken.getExtra() == "\\r");
  }
}

TEST_CASE("Fast Lexer invalid character literal test.") {
  std::string input = "''";
  FastLexer lexer(input);
  lexer.lex();
  REQUIRE(lexer.fail());
  REQUIRE(lexer.getError() ==
          "1:1: error: Invalid character: '''. Lexing Stopped!");
}

TEST_CASE("Fast Lexer line comment test.") {
  auto token_list =
      FastLexer("  aaa//blah\ntest//hehe\r\nmore//test\rtesting").lex();
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
  std::string input = " /*\nee/x";
  FastLexer lexer(input);
  lexer.lex();
  REQUIRE(lexer.fail());
  REQUIRE(lexer.getError() ==
          "1:2: error: Unterminated Comment!. Lexing Stopped!");
}

TEST_CASE("Fast Lexer string empty test.") {
  std::string input;
  auto tokenList = FastLexer(input).lex();
}

TEST_CASE("Fast Lexer string empty string test.") {
  std::string input = "\"\"";
  auto tokenList = FastLexer(input).lex();
  auto &firstToken = tokenList.front();
  REQUIRE(firstToken.getType() == TokenType::STRING);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra().empty());
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
  auto tokenList = FastLexer(R"("strings\\ \n are slow")").lex();
  auto &firstToken = tokenList.front();
  REQUIRE(firstToken.getType() == TokenType::STRING);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  REQUIRE(firstToken.getExtra() == "strings\\\\ \\n are slow");
}

TEST_CASE("Fast Lexer invalid string literal test.") {
  std::string input = "this has invalid escape \\q";
  FastLexer lexer(input);
  lexer.lex();
  REQUIRE(lexer.fail());
}

TEST_CASE("::") {
  auto tokenList = FastLexer(":::").lex();
  REQUIRE(tokenList.size() == 3);
  auto &firstToken = tokenList.front();
  REQUIRE(firstToken.getType() == TokenType::COLON);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  auto &secondToken = tokenList[1];
  REQUIRE(secondToken.getType() == TokenType::COLON);
  REQUIRE(secondToken.getLine() == 1);
  REQUIRE(secondToken.getColumn() == 2);
  auto &thirdToken = tokenList[2];
  REQUIRE(thirdToken.getType() == TokenType::COLON);
  REQUIRE(thirdToken.getLine() == 1);
  REQUIRE(thirdToken.getColumn() == 3);
}

TEST_CASE(".*") {
  auto tokenList = FastLexer(".*.").lex();
  REQUIRE(tokenList.size() == 3);
  auto &firstToken = tokenList.front();
  REQUIRE(firstToken.getType() == TokenType::DOT);
  REQUIRE(firstToken.getLine() == 1);
  REQUIRE(firstToken.getColumn() == 1);
  auto &secondToken = tokenList[1];
  REQUIRE(secondToken.getType() == TokenType::STAR);
  REQUIRE(secondToken.getLine() == 1);
  REQUIRE(secondToken.getColumn() == 2);
  auto &thirdToken = tokenList[2];
  REQUIRE(thirdToken.getType() == TokenType::DOT);
  REQUIRE(thirdToken.getLine() == 1);
  REQUIRE(thirdToken.getColumn() == 3);
}
