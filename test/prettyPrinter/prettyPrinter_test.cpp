#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include "utils/utils.hpp"
#include <fstream>

using namespace ccc;
bool compare(ASTNode *root, const std::string &expected) {
  std::string content = root->prettyPrint();
  if (expected != content) {
    std::vector<std::string> expected_lines = Utils::split_lines(expected);
    std::vector<std::string> content_lines = Utils::split_lines(content);

    for (unsigned long i = 0, errors = 0;
         i < std::max(content_lines.size(), expected_lines.size()); i++) {
      if (i >= expected_lines.size()) {
        std::cerr << "expected nothing but got \"" << content_lines[i] << "\""
                  << std::endl;
      } else if (i >= content_lines.size()) {
        std::cerr << "expected \"" << expected_lines[i] << "\" but got nothing"
                  << std::endl;
      } else if (content_lines[i] != expected_lines[i]) {
        std::cerr << "expected \"" << expected_lines[i] << "\" but got \""
                  << content_lines[i] << "\"" << std::endl;
      } else {
        continue;
      }
      errors++;
      if (errors > 5) {
        std::cerr << std::endl
                  << "Output truncated after five errors... - fix your stuff!"
                  << std::endl;
        break;
      }
    }
    //    std::cout << content << std::endl;
    return false;
  }
  return true;
}
TEST_CASE("pretty print block block") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new CompoundStatement(Token(TokenType::GHOST), {})});

  REQUIRE(compare(root, "{\n"
                        "\t{\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print block") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new ExpressionStatement(
          Token(TokenType::GHOST),
          new BinaryExpression(
              Token(TokenType::ASSIGN),
              new Identifier(Token(TokenType::IDENTIFIER, "b")),
              new Constant(Token(TokenType::NUMBER, "2"))))});

  REQUIRE(compare(root, "{\n"
                        "\t(b = 2);\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new CompoundStatement(
              Token(TokenType::GHOST),
              {new ExpressionStatement(
                  Token(TokenType::GHOST),
                  new BinaryExpression(
                      Token(TokenType::PLUS_ASSIGN),
                      new Identifier(Token(TokenType::IDENTIFIER, "b")),
                      new Constant(Token(TokenType::NUMBER, "2"))))}))});

  REQUIRE(compare(root, "{\n"
                        "\tif ((a == 1)) {\n"
                        "\t\t(b += 2);\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if inline") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new ExpressionStatement(
              Token(TokenType::GHOST),
              new BinaryExpression(
                  Token(TokenType::PLUS_ASSIGN),
                  new Identifier(Token(TokenType::IDENTIFIER, "b")),
                  new Constant(Token(TokenType::NUMBER, "2")))))});

  REQUIRE(compare(root, "{\n"
                        "\tif ((a == 1))\n"
                        "\t\t(b += 2);\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new CompoundStatement(
              Token(TokenType::GHOST),
              {new ExpressionStatement(
                  Token(TokenType::GHOST),
                  new BinaryExpression(
                      Token(TokenType::PLUS_ASSIGN),
                      new Identifier(Token(TokenType::IDENTIFIER, "b")),
                      new Constant(Token(TokenType::NUMBER, "2"))))}),
          new CompoundStatement(
              Token(TokenType::BRACE_OPEN),
              {new ExpressionStatement(
                  Token(TokenType::GHOST),
                  new BinaryExpression(
                      Token(TokenType::PLUS_ASSIGN),
                      new Identifier(Token(TokenType::IDENTIFIER, "b")),
                      new Constant(Token(TokenType::NUMBER, "2"))))}))});

  REQUIRE(compare(root, "{\n"
                        "\tif ((a == 1)) {\n"
                        "\t\t(b += 2);\n"
                        "\t} else {\n"
                        "\t\t(b += 2);\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else inline") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new ExpressionStatement(
              Token(TokenType::GHOST),
              new BinaryExpression(
                  Token(TokenType::PLUS_ASSIGN),
                  new Identifier(Token(TokenType::IDENTIFIER, "b")),
                  new Constant(Token(TokenType::NUMBER, "2")))),
          new ExpressionStatement(
              Token(TokenType::GHOST),
              new BinaryExpression(
                  Token(TokenType::DIV),
                  new Identifier(Token(TokenType::IDENTIFIER, "a")),
                  new Constant(Token(TokenType::NUMBER, "3")))))});

  REQUIRE(compare(root, "{\n"
                        "\tif ((a == 1))\n"
                        "\t\t(b += 2);\n"
                        "\telse\n"
                        "\t\t(a / 3);\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else if else inline") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new Constant(Token(TokenType::NUMBER, "1")),
          new ReturnStatement(
              Token(TokenType::GHOST),
              new BinaryExpression(
                  Token(TokenType::PLUS),
                  new Constant(Token(TokenType::NUMBER, "1")),
                  new Constant(Token(TokenType::NUMBER, "3")))),
          new IfElseStatement(
              Token(TokenType::GHOST),
              new Constant(Token(TokenType::NUMBER, "0")),
              new ReturnStatement(
                  Token(TokenType::GHOST),
                  new Identifier(Token(TokenType::NUMBER, "1"))),
              new CompoundStatement(
                  Token(TokenType::GHOST),
                  {new ReturnStatement(
                      Token(TokenType::GHOST),
                      new Identifier(Token(TokenType::NUMBER, "0")))})))});

  REQUIRE(compare(root, "{\n"
                        "\tif (1)\n"
                        "\t\treturn (1 + 3);\n"
                        "\telse if (0)\n"
                        "\t\treturn 1;\n"
                        "\telse {\n"
                        "\t\treturn 0;\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else if else") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new Constant(Token(TokenType::NUMBER, "1")),
          new ReturnStatement(
              Token(TokenType::GHOST),
              new BinaryExpression(
                  Token(TokenType::PLUS),
                  new Constant(Token(TokenType::NUMBER, "1")),
                  new Constant(Token(TokenType::NUMBER, "3")))),
          new IfElseStatement(
              Token(TokenType::GHOST),
              new Constant(Token(TokenType::NUMBER, "0")),
              new CompoundStatement(
                  Token(TokenType::GHOST),
                  {new ReturnStatement(
                      Token(TokenType::GHOST),
                      new Identifier(Token(TokenType::NUMBER, "1")))}),
              new CompoundStatement(
                  Token(TokenType::GHOST),
                  {new ReturnStatement(
                      Token(TokenType::GHOST),
                      new Identifier(Token(TokenType::NUMBER, "0")))})))});

  REQUIRE(compare(root, "{\n"
                        "\tif (1)\n"
                        "\t\treturn (1 + 3);\n"
                        "\telse if (0) {\n"
                        "\t\treturn 1;\n"
                        "\t} else {\n"
                        "\t\treturn 0;\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else if") {
  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new IfElseStatement(
          Token(TokenType::GHOST),
          new Constant(Token(TokenType::NUMBER, "1")),
          new ReturnStatement(
              Token(TokenType::GHOST),
              new BinaryExpression(
                  Token(TokenType::PLUS),
                  new Constant(Token(TokenType::NUMBER, "1")),
                  new Constant(Token(TokenType::NUMBER, "3")))),
          new IfElseStatement(
              Token(TokenType::GHOST),
              new Constant(Token(TokenType::NUMBER, "0")),
              new CompoundStatement(
                  Token(TokenType::GHOST),
                  {new ReturnStatement(
                      Token(TokenType::GHOST),
                      new Identifier(Token(TokenType::NUMBER, "1")))})))});

  REQUIRE(compare(root, "{\n"
                        "\tif (1)\n"
                        "\t\treturn (1 + 3);\n"
                        "\telse if (0) {\n"
                        "\t\treturn 1;\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print while") {
  auto cond =
      new BinaryExpression(Token(TokenType::PLUS),
                           new Identifier(Token(TokenType::NUMBER, "1")),
                           new Identifier(Token(TokenType::NUMBER, "3")));

  auto stat = new CompoundStatement(
      Token(TokenType::GHOST),
      {new ExpressionStatement(
           Token(TokenType::GHOST),
           new BinaryExpression(
               Token(TokenType::PLUS),
               new Constant(Token(TokenType::NUMBER, "1")),
               new Constant(Token(TokenType::NUMBER, "3")))),
       new ExpressionStatement(
           Token(TokenType::GHOST),
           new BinaryExpression(
               Token(TokenType::STAR),
               new Constant(Token(TokenType::NUMBER, "0")),
               new Constant(Token(TokenType::NUMBER, "5"))))});

  auto root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new WhileStatement(Token(TokenType::GHOST), cond, stat)});

  REQUIRE(compare(root, "{\n"
                        "\twhile ((1 + 3)) {\n"
                        "\t\t(1 + 3);\n"
                        "\t\t(0 * 5);\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print while inline") {
  auto *root = new CompoundStatement(
      Token(TokenType::GHOST),
      {new WhileStatement(
          Token(TokenType::GHOST),
          new Constant(Token(TokenType::NUMBER, "3")),
          new IfElseStatement(
              Token(TokenType::GHOST),
              new Constant(Token(TokenType::NUMBER, "1")),
              new ReturnStatement(
                  Token(TokenType::GHOST),
                  new Identifier(Token(TokenType::NUMBER, "1"))),
              new ReturnStatement(
                  Token(TokenType::GHOST),
                  new Identifier(Token(TokenType::STRING, "\"test\"")))))});

  REQUIRE(compare(root, "{\n"
                        "\twhile (3)\n"
                        "\t\tif (1)\n"
                        "\t\t\treturn 1;\n"
                        "\t\telse\n"
                        "\t\t\treturn \"test\";\n"
                        "}\n"));
  delete root;
}
