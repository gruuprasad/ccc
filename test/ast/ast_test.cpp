#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>
namespace ccc {
// XXX Disabling AST related tests until transition to unique_ptr is completed,
// then we can rewrite these testcases properly
/*
TEST_CASE("ast statement test") {
  auto root = new TranslationUnit({new CompoundStatement(
      Token(),
      {new IfElseStatement(
           Token(), new ConstantExpression(Token(TokenType::NUMBER, "1")),
           new LabeledStatement(
               new IdentifierExpression(Token(TokenType::IDENTIFIER, "foo")),
               new GotoStatement(new IdentifierExpression(
                   Token(TokenType::IDENTIFIER, "empty")))),
           new IfElseStatement(
               Token(), new ConstantExpression(Token(TokenType::NUMBER, "0")),
               new CompoundStatement(
                   Token(), {new GotoStatement(new IdentifierExpression(
                                 Token(TokenType::IDENTIFIER, "end"))),
                             new LabeledStatement(new IdentifierExpression(
                                 Token(TokenType::IDENTIFIER, "bar")))}),
               new CompoundStatement(
                   Token(), {new GotoStatement(new IdentifierExpression(
                                Token(TokenType::IDENTIFIER, "foo")))}))),
       new LabeledStatement(
           new IdentifierExpression(Token(TokenType::IDENTIFIER, "empty")),
           new ExpressionStatement(Token())),
       new LabeledStatement(
           new IdentifierExpression(Token(TokenType::IDENTIFIER, "end")),
           new IfElseStatement(
               Token(), new ConstantExpression(Token(TokenType::NUMBER, "1")),
               new ReturnStatement(Token()))),
       new CompoundStatement(
           Token(),
           {new WhileStatement(
                Token(), new ConstantExpression(Token(TokenType::NUMBER, "3")),
                new CompoundStatement(Token(), {new BreakStatement(Token())})),
            new WhileStatement(
                Token(), new ConstantExpression(Token(TokenType::NUMBER, "2")),
                new CompoundStatement(
                    Token(),
                    {new WhileStatement(
                         Token(),
                         new BinaryExpression(Token(TokenType::PLUS),
                                              new IdentifierExpression(Token(
                                                  TokenType::NUMBER, "1")),
                                              new IdentifierExpression(Token(
                                                  TokenType::NUMBER, "3"))),
                         new CompoundStatement(
                             Token(),
                             {new ExpressionStatement(
                                  Token(), new BinaryExpression(
                                               Token(TokenType::PLUS),
                                               new ConstantExpression(Token(
                                                   TokenType::NUMBER, "1")),
                                               new ConstantExpression(Token(
                                                   TokenType::NUMBER, "3")))),
                              new ExpressionStatement(
                                  Token(),
                                  new BinaryExpression(
                                      Token(TokenType::STAR),
                                      new ConstantExpression(
                                          Token(TokenType::NUMBER, "0")),
                                      new ConstantExpression(
                                          Token(TokenType::NUMBER, "5"))))})),
                     new CompoundStatement(Token(),
                                           {new ContinueStatement(Token())})})),
            new WhileStatement(
                Token(), new ConstantExpression(Token(TokenType::NUMBER, "1")),
                new CompoundStatement(Token(),
                                      {new ContinueStatement(Token())}))})})});

  //  std::cout << root->prettyPrint(0);
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

TEST_CASE("sample test") {
  ASTNode *root = new TranslationUnit({
      new DeclarationStatement(
          Token(),
          new TypeExpression(
              TypeSpec::INT,
              new FunctionTypeExpression(
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "f")),
                  {
                      new TypeExpression(TypeSpec::INT,
                                         new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "x"))),
                      new TypeExpression(TypeSpec::INT,
                                         new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "y"))),
                  })),
          new CompoundStatement(
              Token(),
              {
                  new CompoundStatement(
                      Token(),
                      {
                          new StructStatement(
                              Token(),
                              new IdentifierExpression(
                                  Token(TokenType::IDENTIFIER, "S")),
                              new CompoundStatement(
                                  Token(),
                                  {new DeclarationStatement(
                                      Token(),
                                      new TypeExpression(
                                          TypeSpec::INT,
                                          new IdentifierExpression(Token(
                                              TokenType::IDENTIFIER, "x"))))}),
                              new IdentifierExpression(
                                  Token(TokenType::IDENTIFIER, "s"))),
                          new ExpressionStatement(
                              Token(),
                              new BinaryExpression(
                                  Token(TokenType::ASSIGN),
                                  new PostfixExpression(
                                      Token(TokenType::DOT),
                                      new IdentifierExpression(
                                          Token(TokenType::IDENTIFIER, "s")),
                                      new IdentifierExpression(
                                          Token(TokenType::IDENTIFIER, "x"))),
                                  new IdentifierExpression(
                                      Token(TokenType::IDENTIFIER, "x")))),
                          new DeclarationStatement(
                              Token(),
                              new StructTypeExpression(
                                  new IdentifierExpression(
                                      Token(TokenType::IDENTIFIER, "S")),
                                  new PointerTypeExpression(
                                      new IdentifierExpression(
                                          Token(TokenType::IDENTIFIER, "p"))))),
                      }),
              })),
      new DeclarationStatement(
          Token(), new TypeExpression(
                       TypeSpec::VOID,
                       new PointerTypeExpression(new FunctionTypeExpression(
                           new PointerTypeExpression(new PointerTypeExpression(
                               new IdentifierExpression(
                                   Token(TokenType::IDENTIFIER, "p")))),
                           {new TypeExpression(TypeSpec::INT)})))),
      new DeclarationStatement(
          Token(),
          new TypeExpression(
              TypeSpec::INT,
              new FunctionTypeExpression(new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "main")),
                                         {new TypeExpression(TypeSpec::VOID)})),
          new CompoundStatement(
              Token(),
              {
                  new DeclarationStatement(
                      Token(),
                      new TypeExpression(TypeSpec::INT,
                                         new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "a")))),
                  new DeclarationStatement(
                      Token(),
                      new TypeExpression(TypeSpec::INT,
                                         new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "b")))),
                  new CompoundStatement(
                      Token(),
                      {new ExpressionStatement(
                          Token(), new BinaryExpression(
                                       Token(TokenType::ASSIGN),
                                       new IdentifierExpression(
                                           Token(TokenType::IDENTIFIER, "a")),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "1"))))}),
                  new ExpressionStatement(
                      Token(),
                      new BinaryExpression(
                          Token(TokenType::ASSIGN),
                          new IdentifierExpression(
                              Token(TokenType::IDENTIFIER, "b")),
                          new BinaryExpression(Token(TokenType::STAR),
                                               new IdentifierExpression(Token(
                                                   TokenType::IDENTIFIER, "a")),
                                               new ConstantExpression(Token(
                                                   TokenType::NUMBER, "2"))))),
                  new IfElseStatement(
                      Token(),
                      new BinaryExpression(Token(TokenType::LESS),
                                           new IdentifierExpression(Token(
                                               TokenType::IDENTIFIER, "a")),
                                           new IdentifierExpression(Token(
                                               TokenType::IDENTIFIER, "b"))),
                      new CompoundStatement(Token(), {}),
                      new GotoStatement(new IdentifierExpression(
                          Token(TokenType::IDENTIFIER, "end")))),
                  new LabeledStatement(
                      new IdentifierExpression(
                          Token(TokenType::IDENTIFIER, "bar")),
                      new LabeledStatement(
                          new IdentifierExpression(
                              Token(TokenType::IDENTIFIER, "end")),
                          new ReturnStatement(
                              Token(), new ConstantExpression(
                                           Token(TokenType::NUMBER, "0"))))),
              })),
  });

  std::cout << root->prettyPrint(0);
  REQUIRE(root->prettyPrint(0) ==
          "int (f(int x, int y))\n"
          "{\n"
          "\t{\n"
          "\t\tstruct S\n"
          "\t\t{\n"
          "\t\t\tint x;\n"
          "\t\t} s;\n"
          "\t\t((s.x) = x);\n"
          "\t\tstruct S (*p);\n"
          //          "\t\t(p = (&s));\n"
          //          "\t\tif (1)\n"
          //          "\t\t\treturn (-((*(&(s.x))) = ((sizeof (s.x)) * ((p->x) -
          //          y))));\n"
          //          "\t\telse if (1)\n"
          //          "\t\t\treturn 1;\n"
          //          "\t\telse {\n"
          //          "\t\t\treturn 0;\n"
          //          "\t\t}\n"
          "\t}\n"
          "}\n"
          "\n"
          "void (*((*(*p))(int)));\n"
          "\n"
          "int (main(void))\n"
          "{\n"
          "\tint a;\n"
          "\tint b;\n"
          "\t{\n"
          "\t\t(a = 1);\n"
          "\t}\n"
          "\t(b = (a * 2));\n"
          "\tif ((a < b)) {\n"
          //          "\t\twhile (((a < (sizeof b)) || (!(b == (f(23,
          //          (sizeof(int))))))))\n" "foo:\n"
          //          "\t\t\tbreak;\n"
          //          "\t} else if (0) {\n"
          //          "\t\tchar (*c);\n"
          //          "\t\twhile (1) {\n"
          //          "\t\t\t(c = \"bla\");\n"
          //          "\t\t\tcontinue;\n"
          //          "\t\t}\n"
          "\t} else\n"
          "\t\tgoto end;\n"
          "bar:\n"
          "end:\n"
          "\treturn 0;\n"
          "}\n");
  //  Utils::saveAST(root, "../../ast");
}
*/
} // namespace ccc
