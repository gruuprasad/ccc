#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "parser/fast_parser.hpp"

namespace ccc {
TEST_CASE("pretty_print - types") {

  SECTION("Simpleton") {
    auto root = make_unique<ScalarType>(Token(), ScalarTypeValue::INT);
    REQUIRE(Utils::compare(root->prettyPrint(0), "int"));
  }
  SECTION("Just struct name") {
    auto root = make_unique<StructType>(Token(), "book");
    REQUIRE(Utils::compare(root->prettyPrint(0), "struct book"));
  }
  SECTION("Add member to struct book") {
    auto root = make_unique<StructType>(
        Token(), "book",
        Utils::vector<ExternalDeclarationListType>(make_unique<DataDeclaration>(
            Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
            make_unique<DirectDeclarator>(
                Token(), make_unique<VariableName>(Token(), "name")))));
    REQUIRE(Utils::compare(root->prettyPrint(0), "struct book\n"
                                                 "{\n"
                                                 "\tint name;\n"
                                                 "}"));
  }
}

TEST_CASE("pretty print block block") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(make_unique<CompoundStmt>(
                   Token(), Utils::vector<ASTNodeListType>())));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\t{\n"
                                               "\t}\n"
                                               "}\n"));
}

TEST_CASE("pretty print block") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<ExpressionStmt>(
          Token(), make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                       make_unique<VariableName>(Token(), "b"),
                                       make_unique<Number>(Token(), 2)))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\t(b + 2);\n"
                                               "}\n"));
}

TEST_CASE("pretty print if") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(Token(), BinaryOpValue::EQUAL,
                              make_unique<VariableName>(Token(), "a"),
                              make_unique<Number>(Token(), 1)),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<ASTNodeListType>(make_unique<ExpressionStmt>(
                  Token(),
                  make_unique<Binary>(Token(), BinaryOpValue::MULTIPLY,
                                      make_unique<VariableName>(Token(), "b"),
                                      make_unique<Number>(Token(), 2))))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif ((a == 1)) {\n"
                                               "\t\t(b * 2);\n"
                                               "\t}\n"
                                               "}\n"));
}

TEST_CASE("pretty print if inline") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(make_unique<IfElse>(
                   Token(),
                   make_unique<Binary>(Token(), BinaryOpValue::EQUAL,
                                       make_unique<VariableName>(Token(), "a"),
                                       make_unique<Character>(Token(), 'a')),
                   make_unique<ExpressionStmt>(
                       Token(), make_unique<Binary>(
                                    Token(), BinaryOpValue::ASSIGN,
                                    make_unique<VariableName>(Token(), "b"),
                                    make_unique<Number>(Token(), 2))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif ((a == 'a'))\n"
                                               "\t\t(b = 2);\n"
                                               "}\n"));
}

TEST_CASE("pretty print if else") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(Token(), BinaryOpValue::EQUAL,
                              make_unique<VariableName>(Token(), "a"),
                              make_unique<Number>(Token(), 1)),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<ASTNodeListType>(make_unique<ExpressionStmt>(
                  Token(),
                  make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                      make_unique<VariableName>(Token(), "b"),
                                      make_unique<Number>(Token(), 2))))),
          make_unique<CompoundStmt>(
              Token(TokenType::BRACE_OPEN),
              Utils::vector<ASTNodeListType>(make_unique<ExpressionStmt>(
                  Token(),
                  make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                      make_unique<VariableName>(Token(), "b"),
                                      make_unique<Number>(Token(), 2))))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif ((a == 1)) {\n"
                                               "\t\t(b + 2);\n"
                                               "\t} else {\n"
                                               "\t\t(b + 2);\n"
                                               "\t}\n"
                                               "}\n"));
}

TEST_CASE("pretty print if else inline") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(Token(), BinaryOpValue::EQUAL,
                              make_unique<VariableName>(Token(), "a"),
                              make_unique<Number>(Token(), 1)),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                  make_unique<VariableName>(Token(), "b"),
                                  make_unique<Number>(Token(), 2))),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<Binary>(Token(), BinaryOpValue::MULTIPLY,
                                  make_unique<VariableName>(Token(), "a"),
                                  make_unique<Number>(Token(), 3))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif ((a == 1))\n"
                                               "\t\t(b + 2);\n"
                                               "\telse\n"
                                               "\t\t(a * 3);\n"
                                               "}\n"));
}

TEST_CASE("pretty print if else if else inline") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(), make_unique<Number>(Token(), 1),
          make_unique<Return>(
              Token(), make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                           make_unique<Number>(Token(), 1),
                                           make_unique<Number>(Token(), 3))),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(), 0),
              make_unique<Return>(Token()),
              make_unique<CompoundStmt>(
                  Token(), Utils::vector<ASTNodeListType>(make_unique<Return>(
                               Token(), make_unique<Number>(Token(), 0))))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif (1)\n"
                                               "\t\treturn (1 + 3);\n"
                                               "\telse if (0)\n"
                                               "\t\treturn;\n"
                                               "\telse {\n"
                                               "\t\treturn 0;\n"
                                               "\t}\n"
                                               "}\n"));
}

TEST_CASE("pretty print if else if else") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(), make_unique<Number>(Token(), 1),
          make_unique<Return>(
              Token(), make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                           make_unique<Number>(Token(), 1),
                                           make_unique<Number>(Token(), 3))),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(), 0),
              make_unique<CompoundStmt>(
                  Token(),
                  Utils::vector<ASTNodeListType>(make_unique<Return>(
                      Token(), make_unique<VariableName>(Token(), "a")))),
              make_unique<IfElse>(
                  Token(), make_unique<Number>(Token(), 0),
                  make_unique<Return>(Token(), make_unique<Number>(Token(), 1)),
                  make_unique<Return>(Token(),
                                      make_unique<Number>(Token(), 0)))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif (1)\n"
                                               "\t\treturn (1 + 3);\n"
                                               "\telse if (0) {\n"
                                               "\t\treturn a;\n"
                                               "\t} else if (0)\n"
                                               "\t\treturn 1;\n"
                                               "\telse\n"
                                               "\t\treturn 0;\n"
                                               "}\n"));
}

TEST_CASE("pretty print if else if") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(), make_unique<Number>(Token(), 1),
          make_unique<Return>(
              Token(), make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                           make_unique<Number>(Token(), 1),
                                           make_unique<Number>(Token(), 3))),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(), 0),
              make_unique<CompoundStmt>(
                  Token(), Utils::vector<ASTNodeListType>(make_unique<Return>(
                               Token(), make_unique<Number>(Token(), 1))))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif (1)\n"
                                               "\t\treturn (1 + 3);\n"
                                               "\telse if (0) {\n"
                                               "\t\treturn 1;\n"
                                               "\t}\n"
                                               "}\n"));
}

TEST_CASE("pretty print while") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<While>(
          Token(),
          make_unique<Binary>(Token(), BinaryOpValue::ADD,
                              make_unique<VariableName>(Token(), "a"),
                              make_unique<VariableName>(Token(), "b")),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<ASTNodeListType>(
                  make_unique<ExpressionStmt>(
                      Token(),
                      make_unique<Binary>(Token(), BinaryOpValue::ADD,
                                          make_unique<Number>(Token(), 1),
                                          make_unique<Number>(Token(), 3))),
                  make_unique<ExpressionStmt>(
                      Token(), make_unique<Binary>(
                                   Token(), BinaryOpValue::MULTIPLY,
                                   make_unique<Number>(Token(), 0),
                                   make_unique<Number>(Token(), 5))))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\twhile ((a + b)) {\n"
                                               "\t\t(1 + 3);\n"
                                               "\t\t(0 * 5);\n"
                                               "\t}\n"
                                               "}\n"));
}

TEST_CASE("pretty print while inline") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(make_unique<While>(
                   Token(), make_unique<Number>(Token(), 3),
                   make_unique<IfElse>(
                       Token(), make_unique<Number>(Token(), 1),
                       make_unique<Return>(
                           Token(), make_unique<VariableName>(Token(), "a")),
                       make_unique<Return>(
                           Token(), make_unique<String>(Token(), "test"))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\twhile (3)\n"
                                               "\t\tif (1)\n"
                                               "\t\t\treturn a;\n"
                                               "\t\telse\n"
                                               "\t\t\treturn \"test\";\n"
                                               "}\n"));
}

TEST_CASE("pretty print while inline if else break continue") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(make_unique<While>(
                   Token(), make_unique<Number>(Token(), 3),
                   make_unique<IfElse>(Token(), make_unique<Number>(Token(), 1),
                                       make_unique<Break>(Token()),
                                       make_unique<Continue>(Token())))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\twhile (3)\n"
                                               "\t\tif (1)\n"
                                               "\t\t\tbreak;\n"
                                               "\t\telse\n"
                                               "\t\t\tcontinue;\n"
                                               "}\n"));
}

TEST_CASE("pretty print while inline break continue") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(
                   make_unique<While>(Token(), make_unique<Number>(Token(), 3),
                                      make_unique<Break>(Token())),
                   make_unique<While>(Token(), make_unique<Number>(Token(), 1),
                                      make_unique<Continue>(Token()))));

  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\twhile (3)\n"
                                               "\t\tbreak;\n"
                                               "\twhile (1)\n"
                                               "\t\tcontinue;\n"
                                               "}\n"));
}

TEST_CASE("conditional") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<ExpressionStmt>(
          Token(),
          make_unique<Ternary>(
              Token(),
              make_unique<Binary>(Token(), BinaryOpValue::LESS_THAN,
                                  make_unique<VariableName>(Token(), "a"),
                                  make_unique<Number>(Token(), 0)),
              make_unique<Number>(Token(), 1),
              make_unique<Number>(Token(), 3)))));

  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\t((a < 0) ? 1 : 3);\n"
                                               "}\n"));
}

TEST_CASE("pretty print while inline break continue blocks") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<While>(Token(), make_unique<Number>(Token(), 3),
                             make_unique<CompoundStmt>(
                                 Token(), Utils::vector<ASTNodeListType>(
                                              make_unique<Break>(Token())))),
          make_unique<While>(
              Token(), make_unique<Number>(Token(), 2),
              make_unique<CompoundStmt>(
                  Token(),
                  Utils::vector<ASTNodeListType>(
                      make_unique<Break>(Token()),
                      make_unique<CompoundStmt>(
                          Token(), Utils::vector<ASTNodeListType>(
                                       make_unique<Continue>(Token())))))),
          make_unique<While>(
              Token(), make_unique<Number>(Token(), 1),
              make_unique<CompoundStmt>(Token(),
                                        Utils::vector<ASTNodeListType>(
                                            make_unique<Continue>(Token()))))));

  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
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
}

TEST_CASE("pretty print goto label") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(
                   make_unique<While>(
                       Token(), make_unique<Number>(Token(), 1),
                       make_unique<Label>(
                           Token(), make_unique<VariableName>(Token(), "foo"),
                           make_unique<Break>(Token()))),
                   make_unique<Goto>(
                       Token(), make_unique<VariableName>(Token(), "foo"))));

  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\twhile (1)\n"
                                               "foo:\n"
                                               "\t\tbreak;\n"
                                               "\tgoto foo;\n"
                                               "}\n"));
}

TEST_CASE("pretty print if else if else goto label") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(), 1),
              make_unique<Label>(
                  Token(), make_unique<VariableName>(Token(), "foo"),
                  make_unique<Goto>(
                      Token(), make_unique<VariableName>(Token(), "empty"))),
              make_unique<IfElse>(
                  Token(), make_unique<Number>(Token(), 0),
                  make_unique<CompoundStmt>(
                      Token(),
                      Utils::vector<ASTNodeListType>(make_unique<Goto>(
                          Token(), make_unique<VariableName>(Token(), "end")))),
                  make_unique<CompoundStmt>(
                      Token(), Utils::vector<ASTNodeListType>(make_unique<Goto>(
                                   Token(), make_unique<VariableName>(
                                                Token(), "foo")))))),
          make_unique<Label>(
              Token(), make_unique<VariableName>(Token(), "empty"),
              make_unique<Label>(
                  Token(), make_unique<VariableName>(Token(), "end"),
                  make_unique<IfElse>(Token(), make_unique<Number>(Token(), 1),
                                      make_unique<Return>(Token()))))));

  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                               "\tif (1)\n"
                                               "foo:\n"
                                               "\t\tgoto empty;\n"
                                               "\telse if (0) {\n"
                                               "\t\tgoto end;\n"
                                               "\t} else {\n"
                                               "\t\tgoto foo;\n"
                                               "\t}\n"
                                               "empty:\n"
                                               "end:\n"
                                               "\tif (1)\n"
                                               "\t\treturn;\n"
                                               "}\n"));
}

TEST_CASE("declaration") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
              make_unique<DirectDeclarator>(
                  Token(), make_unique<VariableName>(Token(), "a")))
          //          make_unique<DeclarationStatement>(
          //              Token(),
          //              make_unique<TypeExpression>(
          //                  TypeSpec::INT, make_unique<PointerTypeExpression>(
          //                                     make_unique<VariableName>(Token(),
          //                                     "b")))),
          //          make_unique<DeclarationStatement>(
          //              Token(),
          //              make_unique<TypeExpression>(
          //                  TypeSpec::VOID,
          //                  make_unique<PointerTypeExpression>(
          //                                      make_unique<PointerTypeExpression>(
          //                                          make_unique<VariableName>(Token(
          //                                              TokenType::IDENTIFIER,
          //                                              "c")))))),
          //          make_unique<DeclarationStatement>(
          //              Token(), make_unique<TypeExpression>(
          //                           TypeSpec::VOID,
          //                           make_unique<PointerTypeExpression>(
          //                               make_unique<FunctionTypeExpression>(
          //                                   make_unique<PointerTypeExpression>(
          //                                       make_unique<PointerTypeExpression>(
          //                                           make_unique<VariableName>(Token(
          //                                               TokenType::IDENTIFIER,
          //                                               "d")))),
          //                                   std::move(tmp0))))),
          //          make_unique<DeclarationStatement>(
          //              Token(),
          //              make_unique<TypeExpression>(
          //                  TypeSpec::INT,
          //                  make_unique<FunctionTypeExpression>(
          //                                     make_unique<VariableName>(
          //                                         Token(TokenType::IDENTIFIER,
          //                                         "main")),
          //                                     std::move(tmp1)))),
          //          make_unique<DeclarationStatement>(
          //              Token(),
          //              make_unique<StructTypeExpression>(
          //                  make_unique<VariableName>(Token(), "s"),
          //                  make_unique<PointerTypeExpression>(make_unique<VariableName>(
          //                      Token(TokenType::IDENTIFIER, "p"))))),
          //          make_unique<DeclarationStatement>(
          //              Token(),
          //              make_unique<TypeExpression>(
          //                  TypeSpec::INT, make_unique<PointerTypeExpression>(
          //                                     make_unique<FunctionTypeExpression>(
          //                                         make_unique<VariableName>(Token(
          //                                             TokenType::IDENTIFIER,
          //                                             "test")),
          //                                         std::move(tmp2)))))
          ));

  REQUIRE(Utils::compare(
      root->prettyPrint(0),
      "{\n"
      "\tint a;\n"
      //                                               "\tint (*b);\n"
      //                                               "\tvoid (*(*c));\n"
      //                                               "\tvoid
      //                                               (*((*(*d))(int)));\n"
      //                                               "\tint (main(void));\n"
      //                                               "\tstruct S (*p);\n"
      //                                               "\tint
      //                                               (*(test(char)));\n"
      "}\n"));
}
/*
TEST_CASE("declaration init") {
  std::vector<std::unique_ptr<TypeExpression>> tmp;
  tmp.push_back(std::move(make_unique<TypeExpression>(TypeSpec::VOID)));
  tmp.push_back(std::move(make_unique<TypeExpression>(TypeSpec::INT)));
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<DeclarationStatement>(
          Token(),
          make_unique<TypeExpression>(
              TypeSpec::INT, make_unique<FunctionTypeExpression>(
                                 make_unique<VariableName>(
                                     Token(TokenType::IDENTIFIER, "main")),
                                 std::move(tmp))),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<ASTNodeListType>(make_unique<DeclarationStatement>(
                  Token(),
                  make_unique<TypeExpression>(
                      TypeSpec::INT, make_unique<VariableName>(Token(
                                         TokenType::IDENTIFIER, "d"))))))))

  );
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                   "\tint (main(void, int))\n"
                                   "\t{\n"
                                   "\t\tint d;\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("sizeof") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<ExpressionStmt>(
              Token(), make_unique<SizeOfExpression>(
                           Token(), make_unique<VariableName>(Token(), "b"))),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<SizeOfExpression>(
                  Token(), make_unique<TypeExpression>(TypeSpec::INT))),
          make_unique<ExpressionStmt>(
              Token(), make_unique<SizeOfExpression>(
                           Token(), make_unique<Binary>(
                                        Token(), BinaryOpValue::ADD,
                                        make_unique<Number>(Token(), 0),
                                        make_unique<Number>(Token(), 0))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                   "\t(sizeof b);\n"
                                   "\t(sizeof(int));\n"
                                   "\t(sizeof (0 + 0));\n"
                                   "}\n"));
}

TEST_CASE("struct") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<StructStatement>(
          Token(), make_unique<VariableName>(Token(), "s"),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<ASTNodeListType>(make_unique<DeclarationStatement>(
                  Token(),
                  make_unique<TypeExpression>(
                      TypeSpec::INT, make_unique<VariableName>(
                                         Token(), "x"))))),
          make_unique<VariableName>(Token(), "s"))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                   "\tstruct S\n"
                                   "\t{\n"
                                   "\t\tint x;\n"
                                   "\t} s;\n"
                                   "}\n"));
}

TEST_CASE("postfix") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<PostfixExpression>(
                  Token(TokenType::DOT),
                  make_unique<VariableName>(Token(), "s"),
                  make_unique<VariableName>(
                      Token(), "x"))),
          make_unique<ExpressionStmt>(
              Token(), make_unique<PostfixExpression>(
                           Token(TokenType::ARROW),
                           make_unique<PostfixExpression>(
                               Token(TokenType::DOT),
                               make_unique<PostfixExpression>(
                                   Token(TokenType::ARROW),
                                   make_unique<VariableName>(Token(), "a"),
                                   make_unique<VariableName>(
                                       Token(), "s")),
                               make_unique<VariableName>(
                                   Token(), "x")),
                           make_unique<VariableName>(
                               Token(TokenType::IDENTIFIER, "b")))),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<Binary>(Token(), BinaryOpValue::ASSIGN,
                                  make_unique<PostfixExpression>(
                                      Token(TokenType::DOT),
                                      make_unique<VariableName>(
                                          Token(), "s"),
                                      make_unique<VariableName>(
                                          Token(), "x")),
                                  make_unique<VariableName>(
                                      Token(), "x")))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                   "\t(s.x);\n"
                                   "\t(((a->s).x)->b);\n"
                                   "\t((s.x) = x);\n"
                                   "}\n"));
}

TEST_CASE("unary") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<UnaryExpression>(Token(TokenType::AMPERSAND),
                                           make_unique<VariableName>(Token(
                                               ), "s"))),
          make_unique<Return>(
              Token(), make_unique<UnaryExpression>(
                           Token(TokenType::MINUS),
                           make_unique<UnaryExpression>(
                               Token(), BinaryOpValue::MULTIPLY,
                               make_unique<UnaryExpression>(
                                   Token(TokenType::AMPERSAND),
                                   make_unique<PostfixExpression>(
                                       Token(TokenType::DOT),
                                       make_unique<VariableName>(
                                           Token(), "s"),
                                       make_unique<VariableName>(Token(
                                           ), "x"))))))));
  REQUIRE(Utils::compare(root->prettyPrint(0), "{\n"
                                   "\t(&s);\n"
                                   "\treturn (-(*(&(s.x))));\n"
                                   "}\n"));
}

TEST_CASE("call") {
  auto root = new CompoundStmt(
      Token(),
      {
          new ExpressionStmt(
              Token(),
              new CallExpression(
                  Token(),
                  new VariableNameExpression(Token(TokenType::IDENTIFIER, "f")),
                  {new UnaryExpression(Token(TokenType::MINUS),
                                       new VariableNameExpression(
                                           Token(), "s")),
                   new VariableNameExpression(Token(), 1)})),
      });
  REQUIRE(Utils::compare(root, "{\n"
                        "\t(f((-s), 1));\n"
                        "}\n"));
  delete root;
}
*/
} // namespace ccc
