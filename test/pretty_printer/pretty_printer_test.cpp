#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/pretty_printer.hpp"
#include "parser/fast_parser.hpp"

namespace ccc {

TEST_CASE("Simpleton") {
  auto root = make_unique<ScalarType>(Token(), ScalarTypeValue::INT);
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int"));
}

TEST_CASE("Just struct name") {
  auto root = make_unique<StructType>(
      Token(), make_unique<VariableName>(Token(), "book"));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "struct book"));
}

TEST_CASE("Add member to struct book") {
  auto root = make_unique<StructType>(
      Token(), make_unique<VariableName>(Token(), "book"),
      Utils::vector<ExternalDeclarationListType>(make_unique<DataDeclaration>(
          Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
          make_unique<DirectDeclarator>(
              Token(), make_unique<VariableName>(Token(), "name")))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "struct book\n"
                                                  "{\n"
                                                  "\tint name;\n"
                                                  "}"));
}

TEST_CASE("pretty print block block") {
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<ASTNodeListType>(make_unique<CompoundStmt>(
                   Token(), Utils::vector<ASTNodeListType>())));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
                                                  "\tif ((a == 1)) {\n"
                                                  "\t\t(b * 2);\n"
                                                  "\t}\n"
                                                  "}\n"));
}

TEST_CASE("pretty print if inline") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(Token(), BinaryOpValue::EQUAL,
                              make_unique<VariableName>(Token(), "a"),
                              make_unique<Character>(Token(), 'a')),
          make_unique<ExpressionStmt>(
              Token(), make_unique<Assignment>(
                           Token(), make_unique<VariableName>(Token(), "b"),
                           make_unique<Number>(Token(), 2))))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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

  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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

  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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

  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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

  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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

  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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

/*
TEST_CASE("declaration") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
              make_unique<DirectDeclarator>(
                  Token(), make_unique<VariableName>(Token(), "a"))),
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
              make_unique<PointerDeclarator>(
                  Token(),
                  make_unique<DirectDeclarator>(
                      Token(), make_unique<VariableName>(Token(), "b")))),
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::VOID),
              make_unique<PointerDeclarator>(
                  Token(),
                  make_unique<DirectDeclarator>(
                      Token(), make_unique<VariableName>(Token(), "c")),
                  2)),
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::VOID),
              make_unique<PointerDeclarator>(
                  Token(),
                  make_unique<FunctionDeclarator>(
                      Token(),
                      make_unique<PointerDeclarator>(
                          Token(),
                          make_unique<DirectDeclarator>(
                              Token(), make_unique<VariableName>(Token(), "d")),
                          2),
                      Utils::vector<ParamDeclarationListType>(
                          make_unique<ParamDeclaration>(
                              Token(), make_unique<ScalarType>(
                                           Token(), ScalarTypeValue::INT)))))),
          make_unique<FunctionDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::CHAR),
              make_unique<FunctionDeclarator>(
                  Token(),
                  make_unique<DirectDeclarator>(
                      Token(), make_unique<VariableName>(Token(), "main")),
                  Utils::vector<
                      ParamDeclarationListType>(make_unique<ParamDeclaration>(
                      Token(),
                      make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
                      make_unique<DirectDeclarator>(
                          Token(), make_unique<VariableName>(Token(), "a")))))),
          make_unique<StructDeclaration>(
              Token(), make_unique<StructType>(Token(), "S"),
              make_unique<PointerDeclarator>(
                  Token(),
                  make_unique<DirectDeclarator>(
                      Token(), make_unique<VariableName>(Token(), "p"))))));

        auto pp = PPVisitor{};
REQUIRE_EMPTY(Utils::compare(root->accept(&pp),
                               "{\n"
                               "\tint a;\n"
                               "\tint (*b);\n"
                               "\tvoid (*(*c));\n"
                               "\tvoid (*((*(*d))(int)));\n"
                               "\tchar (main(int a));\n"
                               "\tstruct S (*p);\n"
                               "}\n"));
}

TEST_CASE("declaration init") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<FunctionDefinition>(
          Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
          make_unique<FunctionDeclarator>(
              Token(),
              make_unique<DirectDeclarator>(
                  Token(), make_unique<VariableName>(Token(), "main")),
              Utils::vector<ParamDeclarationListType>(
                  make_unique<ParamDeclaration>(
                      Token(),
                      make_unique<ScalarType>(Token(), ScalarTypeValue::INT)),
                  make_unique<ParamDeclaration>(
                      Token(),
                      make_unique<ScalarType>(Token(), ScalarTypeValue::CHAR),
                      make_unique<PointerDeclarator>(Token())),
                  make_unique<ParamDeclaration>(
                      Token(),
                      make_unique<ScalarType>(Token(), ScalarTypeValue::VOID),
                      make_unique<PointerDeclarator>(
                          Token(),
                          make_unique<DirectDeclarator>(
                              Token(), make_unique<VariableName>(Token(), "a")),
                          2)))),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<ASTNodeListType>(make_unique<DataDeclaration>(
                  Token(),
                  make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
                  make_unique<DirectDeclarator>(
                      Token(), make_unique<VariableName>(Token(), "d"))))))));

        auto pp = PPVisitor{};
REQUIRE_EMPTY(Utils::compare(root->accept(&pp),
                               "{\n"
                               "\tint (main(int, char (*), void (*(*a))))\n"
                               "\t{\n"
                               "\t\tint d;\n"
                               "\t}\n"
                               "}\n"));
}
*/
TEST_CASE("sizeof") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<ExpressionStmt>(
              Token(), make_unique<SizeOf>(
                           Token(), make_unique<VariableName>(Token(), "b"))),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<SizeOf>(Token(), make_unique<ScalarType>(
                                               Token(), ScalarTypeValue::INT))),
          make_unique<ExpressionStmt>(
              Token(), make_unique<SizeOf>(
                           Token(), make_unique<Binary>(
                                        Token(), BinaryOpValue::ADD,
                                        make_unique<Number>(Token(), 0),
                                        make_unique<Number>(Token(), 0))))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
                                                  "\t(sizeof b);\n"
                                                  "\t(sizeof(int));\n"
                                                  "\t(sizeof (0 + 0));\n"
                                                  "}\n"));
}

TEST_CASE("struct") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(
          make_unique<StructDeclaration>(
              Token(), make_unique<StructType>(
                           Token(), make_unique<VariableName>(Token(), "S"))),
          make_unique<StructDeclaration>(
              Token(),
              make_unique<StructType>(
                  Token(), make_unique<VariableName>(Token(), "S"),
                  Utils::vector<
                      ExternalDeclarationListType>(make_unique<DataDeclaration>(
                      Token(),
                      make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
                      make_unique<DirectDeclarator>(
                          Token(), make_unique<VariableName>(Token(), "x"))))),
              make_unique<DirectDeclarator>(
                  Token(), make_unique<VariableName>(Token(), "s")))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
                                                  "\tstruct S;\n"
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
              Token(), make_unique<MemberAccessOp>(
                           Token(), PostFixOpValue::DOT,
                           make_unique<VariableName>(Token(), "s"),
                           make_unique<VariableName>(Token(), "x"))),
          make_unique<ExpressionStmt>(
              Token(), make_unique<MemberAccessOp>(
                           Token(), PostFixOpValue::ARROW,
                           make_unique<MemberAccessOp>(
                               Token(), PostFixOpValue::DOT,
                               make_unique<MemberAccessOp>(
                                   Token(), PostFixOpValue::ARROW,
                                   make_unique<VariableName>(Token(), "a"),
                                   make_unique<VariableName>(Token(), "s")),
                               make_unique<VariableName>(Token(), "x")),
                           make_unique<VariableName>(Token(), "b"))),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<Assignment>(
                  Token(),
                  make_unique<MemberAccessOp>(
                      Token(), PostFixOpValue::DOT,
                      make_unique<VariableName>(Token(), "s"),
                      make_unique<VariableName>(Token(), "x")),
                  make_unique<VariableName>(Token(), "x")))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
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
              make_unique<Unary>(Token(), UnaryOpValue::DEREFERENCE,
                                 make_unique<VariableName>(Token(), "s"))),
          make_unique<ExpressionStmt>(
              Token(),
              make_unique<Unary>(Token(), UnaryOpValue::ADDRESS_OF,
                                 make_unique<VariableName>(Token(), "s"))),
          make_unique<Return>(
              Token(),
              make_unique<Unary>(
                  Token(), UnaryOpValue::MINUS,
                  make_unique<Unary>(
                      Token(), UnaryOpValue::DEREFERENCE,
                      make_unique<Unary>(
                          Token(), UnaryOpValue::ADDRESS_OF,
                          make_unique<ArraySubscriptOp>(
                              Token(), make_unique<VariableName>(Token(), "s"),
                              make_unique<VariableName>(Token(), "x"))))))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
                                                  "\t(*s);\n"
                                                  "\t(&s);\n"
                                                  "\treturn (-(*(&(s[x]))));\n"
                                                  "}\n"));
}

TEST_CASE("call") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<ASTNodeListType>(make_unique<ExpressionStmt>(
          Token(),
          make_unique<FunctionCall>(
              Token(), make_unique<VariableName>(Token(), "f"),
              Utils::vector<ExpressionListType>(
                  make_unique<Unary>(Token(), UnaryOpValue::MINUS,
                                     make_unique<VariableName>(Token(), "s")),
                  make_unique<Number>(Token(), 1))))));
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "{\n"
                                                  "\t(f((-s), 1));\n"
                                                  "}\n"));
}

/*
TEST_CASE("traslation") {
  auto root = make_unique<TranslationUnit>(
      Token(),
      Utils::vector<ExternalDeclarationListType>(
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
              make_unique<DirectDeclarator>(
                  Token(), make_unique<VariableName>(Token(), "a"))),
          make_unique<DataDeclaration>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::CHAR),
              make_unique<DirectDeclarator>(
                  Token(), make_unique<VariableName>(Token(), "b"))),
          make_unique<FunctionDefinition>(
              Token(), make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
              make_unique<FunctionDeclarator>(
                  Token(),
                  make_unique<DirectDeclarator>(
                      Token(), make_unique<VariableName>(Token(), "main")),
                  Utils::vector<ParamDeclarationListType>(
                      make_unique<ParamDeclaration>(
                          Token(), make_unique<ScalarType>(
                                       Token(), ScalarTypeValue::INT)),
                      make_unique<ParamDeclaration>(
                          Token(), make_unique<ScalarType>(
                                       Token(), ScalarTypeValue::CHAR)),
                      make_unique<ParamDeclaration>(
                          Token(),
                          make_unique<ScalarType>(Token(),
                                                  ScalarTypeValue::INT),
                          make_unique<DirectDeclarator>(
                              Token(),
                              make_unique<VariableName>(Token(), "a"))))),
              make_unique<CompoundStmt>(
                  Token(),
                  Utils::vector<ASTNodeListType>(make_unique<DataDeclaration>(
                      Token(),
                      make_unique<ScalarType>(Token(), ScalarTypeValue::INT),
                      make_unique<DirectDeclarator>(
                          Token(),
                          make_unique<VariableName>(Token(), "d"))))))));
        auto pp = PPVisitor{};
REQUIRE_EMPTY(Utils::compare(root->accept(&pp),
                               "int a;\n"
                               "\n"
                               "char b;\n"
                               "\n"
                               "int (main(int, char, int a))\n"
                               "{\n"
                               "\tint d;\n"
                               "}\n"));
}
*/
} // namespace ccc
