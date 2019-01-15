/*
#include "../catch.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include "utils/utils.hpp"

namespace ccc {

bool compare(std::unique_ptr<ASTNode> root, const std::string &expected) {
  std::string content = root->prettyPrint(0);
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

TEST_CASE("pretty_print - types") {

  SECTION("Simpleton") {
    auto root =
        make_unique<ScalarType>(Token(TokenType::INT), ScalarTypeValue::INT);
    REQUIRE(compare(std::move(root), "int"));
  }

  SECTION("Just struct name") {
    auto root = make_unique<StructType>(Token(TokenType::STRUCT), "book");
    REQUIRE(compare(std::move(root), "struct book"));
  }

  SECTION("Add member to struct book") {
    auto root = make_unique<StructType>(
        Token(TokenType::STRUCT), "book",
        Utils::vector<ExternalDeclarationListType>(make_unique<DataDeclaration>(
            Token(TokenType::INT),
            make_unique<ScalarType>(Token(TokenType::INT),
                                    ScalarTypeValue::INT),
            make_unique<DirectDeclarator>(
                Token(TokenType::IDENTIFIER, 0, 0, "name1"),
                make_unique<VariableName>(
                    Token(TokenType::IDENTIFIER, 0, 0, "name1"), "name")))));
    REQUIRE(compare(std::move(root), "struct book\n"
                                     "{\n"
                                     "\tint name;\n"
                                     "}"));
  }
}

TEST_CASE("pretty print block block") {
  auto root =
      new CompoundStatement(Token(), {
                                         new CompoundStatement(Token(), {}),
                                     });

  REQUIRE(compare(root, "{\n"
                        "\t{\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print block") {
  std::vector<std::unique_ptr<Statement>> stmt_list;
  stmt_list.emplace_back(make_unique<ExpressionStatement>(
      Token(), make_unique<BinaryExpression>(
                   Token(TokenType::ASSIGN),
                   make_unique<Identifier>(Token(TokenType::IDENTIFIER, "b")),
                   make_unique<Constant>(Token(TokenType::NUMBER, "2")))));
  auto root = make_unique<CompoundStatement>(Token(), std::move(stmt_list));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\t(b = 2);\n"
                                   "}\n"));
}

TEST_CASE("pretty print if") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(Token(TokenType::EQUAL),
                              make_unique<VariableName>(Token(), "a"),
                              make_unique<Number>(Token(), 1)),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<StatementListType>(make_unique<ExpressionStmt>(
                  Token(),
                  make_unique<Binary>(Token(TokenType::PLUS_ASSIGN),
                                      make_unique<VariableName>(Token(), "b"),
                                      make_unique<Number>(Token(), 2))))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\tif ((a == 1)) {\n"
                                   "\t\t(b += 2);\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("pretty print if inline") {
  auto root = new CompoundStatement(
      Token(),

      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(
              Token(TokenType::EQUAL),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER, "a")),
              make_unique<Number>(Token(TokenType::NUMBER, "1"))),
          make_unique<ExpressionStatement>(
              Token(),
              new BinaryExpression(
                  Token(TokenType::EQUAL),
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "a")),
                  new ConstantExpression(Token(TokenType::NUMBER, "1"))),
              new ExpressionStatement(
                  Token(),
                  new BinaryExpression(
                      Token(TokenType::PLUS_ASSIGN),
                      new IdentifierExpression(
                          Token(TokenType::IDENTIFIER, "b")),
                      new ConstantExpression(Token(TokenType::NUMBER, "2"))))),
      });

  REQUIRE(compare(root, "{\n"
                        "\tif ((a == 1))\n"
                        "\t\t(b += 2);\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else") {
  auto root = new CompoundStatement(
      Token(),

      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(
              Token(TokenType::EQUAL),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER, "a")),
              make_unique<Number>(Token(TokenType::NUMBER, "1"))),
          make_unique<CompoundStmt>(
              Token(),
              new BinaryExpression(
                  Token(TokenType::EQUAL),
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "a")),
                  new ConstantExpression(Token(TokenType::NUMBER, "1"))),
              new CompoundStatement(
                  Token(),
                  {
                      new ExpressionStatement(
                          Token(), new BinaryExpression(
                                       Token(TokenType::PLUS_ASSIGN),
                                       new IdentifierExpression(
                                           Token(TokenType::IDENTIFIER, "b")),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "2")))),
                  }),
              new CompoundStatement(
                  Token(TokenType::BRACE_OPEN),
                  {
                      new ExpressionStatement(
                          Token(), new BinaryExpression(
                                       Token(TokenType::PLUS_ASSIGN),
                                       new IdentifierExpression(
                                           Token(TokenType::IDENTIFIER, "b")),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "2")))),
                  })),
      });

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

      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(
              Token(TokenType::EQUAL),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER, "a")),
              make_unique<Number>(Token(TokenType::NUMBER, "1"))),
          make_unique<ExpressionStatement>(
              Token(),
              make_unique<Binary>(
                  Token(TokenType::PLUS_ASSIGN),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"b")), make_unique<Number>(Token(TokenType::NUMBER, "2")))),
          make_unique<ExpressionStatement>(
              Token(),
              new BinaryExpression(
                  Token(TokenType::EQUAL),
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "a")),
                  new ConstantExpression(Token(TokenType::NUMBER, "1"))),
              new ExpressionStatement(
                  Token(),
                  new BinaryExpression(
                      Token(TokenType::PLUS_ASSIGN),
                      new IdentifierExpression(
                          Token(TokenType::IDENTIFIER, "b")),
                      new ConstantExpression(Token(TokenType::NUMBER, "2")))),
              new ExpressionStatement(
                  Token(),
                  new BinaryExpression(
                      Token(TokenType::DIV),
                      new IdentifierExpression(
                          Token(TokenType::IDENTIFIER, "a")),
                      new ConstantExpression(Token(TokenType::NUMBER, "3"))))),
      });

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

      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
          make_unique<ReturnStatement>(
              Token(),
              make_unique<Binary>(
                  Token(TokenType::PLUS),
                  make_unique<Number>(Token(TokenType::NUMBER, "1")),
                  make_unique<Number>(Token(TokenType::NUMBER, "3")))),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "0")),
              make_unique<ReturnStatement>(
                  Token(),
                  new BinaryExpression(
                      Token(TokenType::PLUS),
                      new ConstantExpression(Token(TokenType::NUMBER, "1")),
                      new ConstantExpression(Token(TokenType::NUMBER, "3")))),
              new IfElseStatement(
                  Token(),
                  new ConstantExpression(Token(TokenType::NUMBER, "0")),
                  new ReturnStatement(Token(), new IdentifierExpression(Token(
                                                   TokenType::NUMBER, "1"))),
                  new CompoundStatement(
                      Token(),
                      {
                          new ReturnStatement(
                              Token(), new IdentifierExpression(
                                           Token(TokenType::NUMBER, "0"))),
                      }))),
      });

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

      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
          make_unique<ReturnStatement>(
              Token(),
              make_unique<Binary>(
                  Token(TokenType::PLUS),
                  make_unique<Number>(Token(TokenType::NUMBER, "1")),
                  make_unique<Number>(Token(TokenType::NUMBER, "3")))),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "0")),
              make_unique<CompoundStmt>(
                  Token(),
Utils::vector<StatementListType>(make_unique<ReturnStatement>( Token(),
make_unique<VariableName>( Token(TokenType::NUMBER, "1"))))),
              make_unique<IfElse>(
                  Token(), make_unique<Number>(Token(TokenType::NUMBER, "0")),
                  make_unique<ReturnStatement>(
                      Token(),
                      new ConstantExpression(Token(TokenType::NUMBER, "0")),
                      new ReturnStatement(Token(),
                                          new IdentifierExpression(
                                              Token(TokenType::NUMBER, "1"))),
                      new ReturnStatement(
                          Token(), new IdentifierExpression(
                                       Token(TokenType::NUMBER, "0")))))),
      });

  REQUIRE(compare(root, "{\n"
                        "\tif (1)\n"
                        "\t\treturn (1 + 3);\n"
                        "\telse if (0) {\n"
                        "\t\treturn 1;\n"
                        "\t} else if (0)\n"
                        "\t\treturn 1;\n"
                        "\telse\n"
                        "\t\treturn 0;\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print if else if") {
  auto root = new CompoundStatement(
      Token(),

      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
          make_unique<ReturnStatement>(
              Token(),
              make_unique<Binary>(
                  Token(TokenType::PLUS),
                  make_unique<Number>(Token(TokenType::NUMBER, "1")),
                  make_unique<Number>(Token(TokenType::NUMBER, "3")))),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "0")),
              make_unique<CompoundStmt>(
                  Token(),
                  new ConstantExpression(Token(TokenType::NUMBER, "0")),
                  new CompoundStatement(
                      Token(),
                      {
                          new ReturnStatement(
                              Token(), new IdentifierExpression(
                                           Token(TokenType::NUMBER, "1"))),
                      }))),
      });

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

  auto root = new CompoundStatement(
      Token(),
      {
          new WhileStatement(
              Token(),
              new BinaryExpression(
                  Token(TokenType::PLUS),
                  new IdentifierExpression(Token(TokenType::NUMBER, "1")),
                  new IdentifierExpression(Token(TokenType::NUMBER, "3"))),
              new CompoundStatement(
                  Token(),
                  {
                      new ExpressionStatement(
                          Token(), new BinaryExpression(
                                       Token(TokenType::PLUS),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "1")),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "3")))),
                      new ExpressionStatement(
                          Token(), new BinaryExpression(
                                       Token(TokenType::STAR),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "0")),
                                       new ConstantExpression(
                                           Token(TokenType::NUMBER, "5")))),
                  })),
      });

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

      Utils::vector<StatementListType>(make_unique<WhileStatement>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "3")),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<ReturnStatement>(
                  Token(),
                  new ConstantExpression(Token(TokenType::NUMBER, "1")),
                  new ReturnStatement(Token(), new IdentifierExpression(Token(
                                                   TokenType::NUMBER, "1"))),
                  new ReturnStatement(Token(),
                                      new StringLiteral(Token(TokenType::STRING,
                                                              "\"test\""))))),
      });

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

      Utils::vector<StatementListType>(make_unique<WhileStatement>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "3")),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<BreakStatement>(Token()),
              make_unique<ContinueStatement>(Token())))));

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
      {
          new WhileStatement(
              Token(), new ConstantExpression(Token(TokenType::NUMBER, "3")),
              new BreakStatement(Token())),
          new WhileStatement(
              Token(), new ConstantExpression(Token(TokenType::NUMBER, "1")),
              new ContinueStatement(Token())),
      });

  REQUIRE(compare(root, "{\n"
                        "\twhile (3)\n"
                        "\t\tbreak;\n"
                        "\twhile (1)\n"
                        "\t\tcontinue;\n"
                        "}\n"));
  delete root;
}

TEST_CASE("conditional") {
  auto *root = new CompoundStatement(
      Token(),
      {new ExpressionStatement(
          Token(),
          new ConditionalExpression(
              Token(),
              new BinaryExpression(
                  Token(TokenType::LESS),
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "a")),
                  new ConstantExpression(Token(TokenType::NUMBER, "0"))),
              new ConstantExpression(Token(TokenType::NUMBER, "1")),
              new ConstantExpression(Token(TokenType::NUMBER, "3"))))});

  REQUIRE(compare(root, "{\n"
                        "\t((a < 0) ? 1 : 3);\n"
                        "}\n"));
  delete root;
}

TEST_CASE("pretty print while inline break continue blocks") {
  auto *root = new CompoundStatement(
      Token(),
      {
          new WhileStatement(
              Token(), new ConstantExpression(Token(TokenType::NUMBER, "3")),
              new CompoundStatement(Token(),
                                    {
                                        new BreakStatement(Token()),
                                    })),
          new WhileStatement(
              Token(), new ConstantExpression(Token(TokenType::NUMBER, "2")),
              new CompoundStatement(
                  Token(),
                  {
                      new BreakStatement(Token()),
                      new CompoundStatement(Token(),
                                            {
                                                new ContinueStatement(Token()),
                                            }),
                  })),
          new WhileStatement(
              Token(), new ConstantExpression(Token(TokenType::NUMBER, "1")),
              new CompoundStatement(Token(),
                                    {
                                        new ContinueStatement(Token()),
                                    })),
      });

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
  delete root;
}

TEST_CASE("pretty print goto label") {
  auto *root = new CompoundStatement(
      Token(),
      {
          new WhileStatement(
              Token(), new ConstantExpression(Token(TokenType::NUMBER, "1")),
              new LabeledStatement(
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "foo")),
                  new BreakStatement(Token()))),
          new GotoStatement(
              new IdentifierExpression(Token(TokenType::IDENTIFIER, "foo"))),
      });

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

      Utils::vector<StatementListType>(
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<LabeledStatement>(
                  Token(),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"foo")), make_unique<GotoStatement>( Token(), make_unique<VariableName>(
                                   Token(TokenType::IDENTIFIER, "empty")))),
              make_unique<IfElse>(
                  Token(), make_unique<Number>(Token(TokenType::NUMBER, "0")),
                  make_unique<CompoundStmt>(
                      Token(),
                      Utils::vector<StatementListType>(make_unique<GotoStatement>(
                                 Token(), make_unique<VariableName>(Token(
                                              TokenType::IDENTIFIER, "end"))),
                             make_unique<LabeledStatement>(
                                 Token(), make_unique<VariableName>(Token(
                                              TokenType::IDENTIFIER,
"bar"))))), make_unique<CompoundStmt>( Token(),
                      Utils::vector<StatementListType>(make_unique<GotoStatement>(
                          Token(), make_unique<VariableName>(Token(
                                       TokenType::IDENTIFIER, "foo"))))))),
          make_unique<LabeledStatement>(
              Token(),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"empty")), make_unique<ExpressionStatement>(Token())),
          make_unique<LabeledStatement>(
              Token(),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER, "end")),
              make_unique<IfElse>(
                  Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
                  make_unique<ReturnStatement>(Token())))));
>>>>>>> f1c7734... added a lot of pretty print stuff

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
                        "\tif (1)\n"
                        "\t\treturn;\n"
                        "}\n"));
  delete root;
}

TEST_CASE("declaration") {
  auto root = new CompoundStatement(
      Token(),
      {
          new DeclarationStatement(
              Token(), new TypeExpression(TypeSpec::INT,
                                          new IdentifierExpression(Token(
                                              TokenType::IDENTIFIER, "a")))),
          new DeclarationStatement(
              Token(), new TypeExpression(
                           TypeSpec::INT,
                           new PointerTypeExpression(new IdentifierExpression(
                               Token(TokenType::IDENTIFIER, "b"))))),
          new DeclarationStatement(
              Token(), new TypeExpression(
                           TypeSpec::VOID,
                           new PointerTypeExpression(new PointerTypeExpression(
                               new IdentifierExpression(
                                   Token(TokenType::IDENTIFIER, "c")))))),
          new DeclarationStatement(
              Token(),
              new TypeExpression(
                  TypeSpec::VOID,
                  new PointerTypeExpression(new FunctionTypeExpression(
                      new PointerTypeExpression(
                          new PointerTypeExpression(new IdentifierExpression(
                              Token(TokenType::IDENTIFIER, "d")))),
                      {
                          new TypeExpression(TypeSpec::INT),
                      })))),
          new DeclarationStatement(
              Token(),
              new TypeExpression(TypeSpec::INT,
                                 new FunctionTypeExpression(
                                     new IdentifierExpression(
                                         Token(TokenType::IDENTIFIER, "main")),
                                     std::vector<TypeExpression *>({
                                         new TypeExpression(TypeSpec::VOID),
                                     })))),
          new DeclarationStatement(
              Token(),
              new StructTypeExpression(
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "S")),
                  new PointerTypeExpression(new IdentifierExpression(
                      Token(TokenType::IDENTIFIER, "p"))))),
          new DeclarationStatement(
              Token(), new TypeExpression(
                           TypeSpec::INT,
                           new PointerTypeExpression(new FunctionTypeExpression(
                               new IdentifierExpression(
                                   Token(TokenType::IDENTIFIER, "test")),
                               std::vector<TypeExpression *>({
                                   new TypeExpression(TypeSpec::CHAR),
                               }))))),
      });
  REQUIRE(compare(root, "{\n"
                        "\tint a;\n"
                        "\tint (*b);\n"
                        "\tvoid (*(*c));\n"
                        "\tvoid (*((*(*d))(int)));\n"
                        "\tint (main(void));\n"
                        "\tstruct S (*p);\n"
                        "\tint (*(test(char)));\n"
                        "}\n"));
  delete root;
}

TEST_CASE("declaration init") {
  auto root = new CompoundStatement(
      Token(),
      {new DeclarationStatement(
          Token(),
          new TypeExpression(TypeSpec::INT,
                             new FunctionTypeExpression(
                                 new IdentifierExpression(
                                     Token(TokenType::IDENTIFIER, "main")),
                                 std::vector<TypeExpression *>(
                                     {new TypeExpression(TypeSpec::VOID),
                                      new TypeExpression(TypeSpec::INT)}))),
          new CompoundStatement(
              Token(), {new DeclarationStatement(
                           Token(), new TypeExpression(
                                        TypeSpec::INT,
                                        new IdentifierExpression(Token(
                                            TokenType::IDENTIFIER, "d"))))}))

      });
  REQUIRE(compare(root, "{\n"
                        "\tint (main(void, int))\n"
                        "\t{\n"
                        "\t\tint d;\n"
                        "\t}\n"
                        "}\n"));
  delete root;
}

TEST_CASE("sizeof") {
  auto root = new CompoundStatement(
      Token(),
      {
          new ExpressionStatement(
              Token(),
              new SizeOfExpression(Token(), new IdentifierExpression(Token(
                                                TokenType::IDENTIFIER, "b")))),
          new ExpressionStatement(
              Token(),
              new SizeOfExpression(Token(), new TypeExpression(TypeSpec::INT))),
          new ExpressionStatement(
              Token(),
              new SizeOfExpression(
                  Token(),
                  new BinaryExpression(
                      Token(TokenType::PLUS),
                      new ConstantExpression(Token(TokenType::NUMBER, "0")),
                      new ConstantExpression(Token(TokenType::NUMBER, "0"))))),
      });
  REQUIRE(compare(root, "{\n"
                        "\t(sizeof b);\n"
                        "\t(sizeof(int));\n"
                        "\t(sizeof (0 + 0));\n"
                        "}\n"));
  delete root;
}

TEST_CASE("struct") {
  auto root = new CompoundStatement(
      Token(),
      {new StructStatement(
          Token(), new IdentifierExpression(Token(TokenType::IDENTIFIER, "S")),
          new CompoundStatement(
              Token(), {new DeclarationStatement(
                           Token(), new TypeExpression(
                                        TypeSpec::INT,
                                        new IdentifierExpression(Token(
                                            TokenType::IDENTIFIER, "x"))))}),
          new IdentifierExpression(Token(TokenType::IDENTIFIER, "s")))});
  REQUIRE(compare(root, "{\n"
                        "\tstruct S\n"
                        "\t{\n"
                        "\t\tint x;\n"
                        "\t} s;\n"
                        "}\n"));
  delete root;
}

TEST_CASE("postfix") {
  auto root = new CompoundStatement(
      Token(),
      {new ExpressionStatement(
           Token(),
           new PostfixExpression(
               Token(TokenType::DOT),
               new IdentifierExpression(Token(TokenType::IDENTIFIER, "s")),
               new IdentifierExpression(Token(TokenType::IDENTIFIER, "x")))),
       new ExpressionStatement(
           Token(),
           new PostfixExpression(
               Token(TokenType::ARROW),
               new PostfixExpression(
                   Token(TokenType::DOT),
                   new PostfixExpression(Token(TokenType::ARROW),
                                         new IdentifierExpression(
                                             Token(TokenType::IDENTIFIER, "a")),
                                         new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "s"))),
                   new IdentifierExpression(Token(TokenType::IDENTIFIER, "x"))),
               new IdentifierExpression(Token(TokenType::IDENTIFIER, "b")))),
       new ExpressionStatement(
           Token(),
           new BinaryExpression(
               Token(TokenType::ASSIGN),
               new PostfixExpression(
                   Token(TokenType::DOT),
                   new IdentifierExpression(Token(TokenType::IDENTIFIER, "s")),
                   new IdentifierExpression(Token(TokenType::IDENTIFIER, "x"))),
               new IdentifierExpression(Token(TokenType::IDENTIFIER, "x"))))});
  REQUIRE(compare(root, "{\n"
                        "\t(s.x);\n"
                        "\t(((a->s).x)->b);\n"
                        "\t((s.x) = x);\n"
                        "}\n"));
  delete root;
}

TEST_CASE("unary") {
  auto root = new CompoundStatement(
      Token(),
      {
          new ExpressionStatement(
              Token(), new UnaryExpression(Token(TokenType::AMPERSAND),
                                           new IdentifierExpression(Token(
                                               TokenType::IDENTIFIER, "s")))),
          new ReturnStatement(
              Token(), new UnaryExpression(
                           Token(TokenType::MINUS),
                           new UnaryExpression(
                               Token(TokenType::STAR),
                               new UnaryExpression(
                                   Token(TokenType::AMPERSAND),
                                   new PostfixExpression(
                                       Token(TokenType::DOT),
                                       new IdentifierExpression(
                                           Token(TokenType::IDENTIFIER, "s")),
                                       new IdentifierExpression(Token(
                                           TokenType::IDENTIFIER, "x"))))))),
      });
  REQUIRE(compare(root, "{\n"
                        "\t(&s);\n"
                        "\treturn (-(*(&(s.x))));\n"
                        "}\n"));
  delete root;
}

TEST_CASE("call") {
  auto root = new CompoundStatement(
      Token(),
      {
          new ExpressionStatement(
              Token(),
              new CallExpression(
                  Token(),
                  new IdentifierExpression(Token(TokenType::IDENTIFIER, "f")),
                  {new UnaryExpression(Token(TokenType::MINUS),
                                       new IdentifierExpression(
                                           Token(TokenType::IDENTIFIER, "s"))),
                   new IdentifierExpression(Token(TokenType::NUMBER, "1"))})),
      });
  REQUIRE(compare(root, "{\n"
                        "\t(f((-s), 1));\n"
                        "}\n"));
  delete root;
}
} // namespace ccc
*/
