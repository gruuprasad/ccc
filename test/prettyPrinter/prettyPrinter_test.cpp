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
    std::cout << content << std::endl;
    return false;
  }
  return true;
}
TEST_CASE("pretty print block block") {
  auto root =
      new CompoundStatement(Token(), {new CompoundStatement(Token(), {})});

  REQUIRE(compare(root, "{\n"
                        "\t{\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print block") {
  auto root = new CompoundStatement(
      Token(),
      {new ExpressionStatement(
          Token(), new BinaryExpression(
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
      Token(),
      {new IfElseStatement(
          Token(),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new CompoundStatement(
              Token(),
              {new ExpressionStatement(
                  Token(),
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
      Token(),
      {new IfElseStatement(
          Token(),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new ExpressionStatement(
              Token(), new BinaryExpression(
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
      Token(),
      {new IfElseStatement(
          Token(),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new CompoundStatement(
              Token(),
              {new ExpressionStatement(
                  Token(),
                  new BinaryExpression(
                      Token(TokenType::PLUS_ASSIGN),
                      new Identifier(Token(TokenType::IDENTIFIER, "b")),
                      new Constant(Token(TokenType::NUMBER, "2"))))}),
          new CompoundStatement(
              Token(TokenType::BRACE_OPEN),
              {new ExpressionStatement(
                  Token(),
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
      Token(),
      {new IfElseStatement(
          Token(),
          new BinaryExpression(
              Token(TokenType::EQUAL),
              new Identifier(Token(TokenType::IDENTIFIER, "a")),
              new Constant(Token(TokenType::NUMBER, "1"))),
          new ExpressionStatement(
              Token(), new BinaryExpression(
                           Token(TokenType::PLUS_ASSIGN),
                           new Identifier(Token(TokenType::IDENTIFIER, "b")),
                           new Constant(Token(TokenType::NUMBER, "2")))),
          new ExpressionStatement(
              Token(), new BinaryExpression(
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
      Token(),
      {new IfElseStatement(
          Token(), new Constant(Token(TokenType::NUMBER, "1")),
          new ReturnStatement(Token(),
                              new BinaryExpression(
                                  Token(TokenType::PLUS),
                                  new Constant(Token(TokenType::NUMBER, "1")),
                                  new Constant(Token(TokenType::NUMBER, "3")))),
          new IfElseStatement(
              Token(), new Constant(Token(TokenType::NUMBER, "0")),
              new ReturnStatement(
                  Token(), new Identifier(Token(TokenType::NUMBER, "1"))),
              new CompoundStatement(
                  Token(), {new ReturnStatement(
                               Token(), new Identifier(Token(TokenType::NUMBER,
                                                             "0")))})))});

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
      Token(),
      {new IfElseStatement(
          Token(), new Constant(Token(TokenType::NUMBER, "1")),
          new ReturnStatement(Token(),
                              new BinaryExpression(
                                  Token(TokenType::PLUS),
                                  new Constant(Token(TokenType::NUMBER, "1")),
                                  new Constant(Token(TokenType::NUMBER, "3")))),
          new IfElseStatement(
              Token(), new Constant(Token(TokenType::NUMBER, "0")),
              new CompoundStatement(
                  Token(),
                  {new ReturnStatement(
                      Token(), new Identifier(Token(TokenType::NUMBER, "1")))}),
              new CompoundStatement(
                  Token(), {new ReturnStatement(
                               Token(), new Identifier(Token(TokenType::NUMBER,
                                                             "0")))})))});

  REQUIRE(compare(root, "{\n"
                        "\tif (1)\n"
                        "\t\treturn (1 + 3);\n"
                        "\telse if (0) {\n"
                        "\t\treturn 1;\n"
                        "\t} else {\n"
                        "\t\treturn 0;\n"
                        "\t}\n"
                        "}\n"));
  //  Utils::saveAST(root, "test.gv");
  delete root;
}

TEST_CASE("pretty print if else if") {
  auto root = new CompoundStatement(
      Token(),
      {new IfElseStatement(
          Token(), new Constant(Token(TokenType::NUMBER, "1")),
          new ReturnStatement(Token(),
                              new BinaryExpression(
                                  Token(TokenType::PLUS),
                                  new Constant(Token(TokenType::NUMBER, "1")),
                                  new Constant(Token(TokenType::NUMBER, "3")))),
          new IfElseStatement(
              Token(), new Constant(Token(TokenType::NUMBER, "0")),
              new CompoundStatement(
                  Token(), {new ReturnStatement(
                               Token(), new Identifier(Token(TokenType::NUMBER,
                                                             "1")))})))});

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
  auto cond = new BinaryExpression(
      Token(TokenType::PLUS), new Identifier(Token(TokenType::NUMBER, "1")),
      new Identifier(Token(TokenType::NUMBER, "3")));

  auto stat = new CompoundStatement(
      Token(),
      {new ExpressionStatement(
           Token(),
           new BinaryExpression(Token(TokenType::PLUS),
                                new Constant(Token(TokenType::NUMBER, "1")),
                                new Constant(Token(TokenType::NUMBER, "3")))),
       new ExpressionStatement(
           Token(),
           new BinaryExpression(Token(TokenType::STAR),
                                new Constant(Token(TokenType::NUMBER, "0")),
                                new Constant(Token(TokenType::NUMBER, "5"))))});

  auto root =
      new CompoundStatement(Token(), {new WhileStatement(Token(), cond, stat)});

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
      Token(),
      {new WhileStatement(
          Token(), new Constant(Token(TokenType::NUMBER, "3")),
          new IfElseStatement(
              Token(), new Constant(Token(TokenType::NUMBER, "1")),
              new ReturnStatement(
                  Token(), new Identifier(Token(TokenType::NUMBER, "1"))),
              new ReturnStatement(
                  Token(),
                  new StringLiteral(Token(TokenType::STRING, "\"test\"")))))});

  REQUIRE(compare(root, "{\n"
                        "\twhile (3)\n"
                        "\t\tif (1)\n"
                        "\t\t\treturn 1;\n"
                        "\t\telse\n"
                        "\t\t\treturn \"test\";\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print while inline if else break continue") {
  auto *root = new CompoundStatement(
      Token(),
      {new WhileStatement(
          Token(), new Constant(Token(TokenType::NUMBER, "3")),
          new IfElseStatement(
              Token(), new Constant(Token(TokenType::NUMBER, "1")),
              new BreakStatement(Token()), new ContinueStatement(Token())))});

  REQUIRE(compare(root, "{\n"
                        "\twhile (3)\n"
                        "\t\tif (1)\n"
                        "\t\t\tbreak;\n"
                        "\t\telse\n"
                        "\t\t\tcontinue;\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print while inline break continue") {
  auto *root = new CompoundStatement(
      Token(),
      {new WhileStatement(Token(), new Constant(Token(TokenType::NUMBER, "3")),
                          new BreakStatement(Token())),
       new WhileStatement(Token(), new Constant(Token(TokenType::NUMBER, "1")),
                          new ContinueStatement(Token()))});

  REQUIRE(compare(root, "{\n"
                        "\twhile (3)\n"
                        "\t\tbreak;\n"
                        "\twhile (1)\n"
                        "\t\tcontinue;\n"
                        "}\n"));
  delete root;
}
TEST_CASE("pretty print while inline break continue blocks") {
  auto *root = new CompoundStatement(
      Token(),
      {new WhileStatement(
           Token(), new Constant(Token(TokenType::NUMBER, "3")),
           new CompoundStatement(Token(), {new BreakStatement(Token())})),
       new WhileStatement(
           Token(), new Constant(Token(TokenType::NUMBER, "2")),
           new CompoundStatement(
               Token(), {new BreakStatement(Token()),
                         new CompoundStatement(
                             Token(), {new ContinueStatement(Token())})})),
       new WhileStatement(
           Token(), new Constant(Token(TokenType::NUMBER, "1")),
           new CompoundStatement(Token(), {new ContinueStatement(Token())}))});

  REQUIRE(compare(root, "{\n"
                        "\twhile (3) {\n"
                        "\t\tbreak;\n"
                        "\t}\n"
                        "\twhile (2) {\n"
                        "\t\tbreak;\n"
                        "\t\t{\n"
                        "\t\t\tcontinue;\n"
                        "\t\t}\n"
                        "\t}\n"
                        "\twhile (1) {\n"
                        "\t\tcontinue;\n"
                        "\t}\n"
                        "}\n"));
  //  Utils::saveAST(root, "test.gv");
  delete root;
}

TEST_CASE("pretty print goto label") {
  auto *root = new CompoundStatement(
      Token(),
      {new WhileStatement(
           Token(), new Constant(Token(TokenType::NUMBER, "1")),
           new LabeledStatement(
               new Identifier(Token(TokenType::IDENTIFIER, "foo")),
               new BreakStatement(Token()))),
       new GotoStatement(new Identifier(Token(TokenType::IDENTIFIER, "foo")))});

  REQUIRE(compare(root, "{\n"
                        "\twhile (1)\n"
                        "foo:\n"
                        "\t\tbreak;\n"
                        "\tgoto foo;\n"
                        "}\n"));
  delete root;
}
TEST_CASE("pretty print if else if else goto label") {
  auto root = new CompoundStatement(
      Token(),
      {new IfElseStatement(
           Token(), new Constant(Token(TokenType::NUMBER, "1")),
           new LabeledStatement(
               new Identifier(Token(TokenType::IDENTIFIER, "foo")),
               new GotoStatement(
                   new Identifier(Token(TokenType::IDENTIFIER, "empty")))),
           new IfElseStatement(
               Token(), new Constant(Token(TokenType::NUMBER, "0")),
               new CompoundStatement(
                   Token(), {new GotoStatement(new Identifier(
                                 Token(TokenType::IDENTIFIER, "end"))),
                             new LabeledStatement(new Identifier(
                                 Token(TokenType::IDENTIFIER, "bar")))}),
               new CompoundStatement(
                   Token(), {new GotoStatement(new Identifier(
                                Token(TokenType::IDENTIFIER, "foo")))}))),
       new LabeledStatement(
           new Identifier(Token(TokenType::IDENTIFIER, "empty")),
           new ExpressionStatement(Token())),
       new LabeledStatement(
           new Identifier(Token(TokenType::IDENTIFIER, "end")))});

  REQUIRE(compare(root, "{\n"
                        "\tif (1)\n"
                        "foo:\n"
                        "\t\tgoto empty;\n"
                        "\telse if (0) {\n"
                        "\t\tgoto end;\n"
                        "bar:\n"
                        "\t} else {\n"
                        "\t\tgoto foo;\n"
                        "\t}\n"
                        "empty:\n"
                        "\t;\n"
                        "end:\n"
                        "}\n"));
  //  Utils::saveAST(root, "../../ast");
  delete root;
}
