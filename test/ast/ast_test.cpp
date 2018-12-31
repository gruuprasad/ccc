#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>
namespace ccc {

TEST_CASE("ast statement test") {
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
           new Identifier(Token(TokenType::IDENTIFIER, "end")),
           new IfElseStatement(Token(),
                               new Constant(Token(TokenType::NUMBER, "1")),
                               new ReturnStatement(Token()))),
       new CompoundStatement(
           Token(),
           {new WhileStatement(
                Token(), new Constant(Token(TokenType::NUMBER, "3")),
                new CompoundStatement(Token(), {new BreakStatement(Token())})),
            new WhileStatement(
                Token(), new Constant(Token(TokenType::NUMBER, "2")),
                new CompoundStatement(
                    Token(),
                    {new WhileStatement(
                         Token(),
                         new BinaryExpression(
                             Token(TokenType::PLUS),
                             new Identifier(Token(TokenType::NUMBER, "1")),
                             new Identifier(Token(TokenType::NUMBER, "3"))),
                         new CompoundStatement(
                             Token(),
                             {new ExpressionStatement(
                                  Token(), new BinaryExpression(
                                               Token(TokenType::PLUS),
                                               new Constant(Token(
                                                   TokenType::NUMBER, "1")),
                                               new Constant(Token(
                                                   TokenType::NUMBER, "3")))),
                              new ExpressionStatement(
                                  Token(),
                                  new BinaryExpression(
                                      Token(TokenType::STAR),
                                      new Constant(
                                          Token(TokenType::NUMBER, "0")),
                                      new Constant(
                                          Token(TokenType::NUMBER, "5"))))})),
                     new CompoundStatement(Token(),
                                           {new ContinueStatement(Token())})})),
            new WhileStatement(
                Token(), new Constant(Token(TokenType::NUMBER, "1")),
                new CompoundStatement(Token(),
                                      {new ContinueStatement(Token())}))})});

  std::cout << root->prettyPrint(0);
  REQUIRE(root->prettyPrint(0) == "{\n"
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
                                  "\tif (1)\n"
                                  "\t\treturn;\n"
                                  "\t{\n"
                                  "\t\twhile (3) {\n"
                                  "\t\t\tbreak;\n"
                                  "\t\t}\n"
                                  "\t\twhile (2) {\n"
                                  "\t\t\twhile ((1 + 3)) {\n"
                                  "\t\t\t\t(1 + 3);\n"
                                  "\t\t\t\t(0 * 5);\n"
                                  "\t\t\t}\n"
                                  "\t\t\t{\n"
                                  "\t\t\t\tcontinue;\n"
                                  "\t\t\t}\n"
                                  "\t\t}\n"
                                  "\t\twhile (1) {\n"
                                  "\t\t\tcontinue;\n"
                                  "\t\t}\n"
                                  "\t}\n"
                                  "}\n");
  //  Utils::saveAST(root, "../../ast");
}
} // namespace ccc
