#include "../catch.hpp"
#include "ast/ast_node.hpp"
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
  auto root = make_unique<CompoundStmt>(
      Token(), Utils::vector<StatementListType>(make_unique<CompoundStmt>(
                   Token(), Utils::vector<StatementListType>())));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\t{\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("pretty print block") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<ExpressionStmt>(
          Token(), make_unique<Binary>(Token(TokenType::PLUS),
                                       make_unique<VariableName>(Token(), "b"),
                                       make_unique<Number>(Token(), 2)))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\t(b + 2);\n"
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
/*
TEST_CASE("pretty print if inline") {
  auto root = make_unique<CompoundStmt>(
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
"b")), make_unique<Number>(Token(TokenType::NUMBER, "2")))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\tif ((a == 1))\n"
                                   "\t\t(b += 2);\n"
                                   "}\n"));
}

TEST_CASE("pretty print if else") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<IfElse>(
          Token(),
          make_unique<Binary>(
              Token(TokenType::EQUAL),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER, "a")),
              make_unique<Number>(Token(TokenType::NUMBER, "1"))),
          make_unique<CompoundStmt>(
              Token(),
Utils::vector<StatementListType>(make_unique<ExpressionStatement>( Token(),
make_unique<Binary>( Token(TokenType::PLUS_ASSIGN),
                                        make_unique<VariableName>(
                                            Token(TokenType::IDENTIFIER,
"b")), make_unique<Number>( Token(TokenType::NUMBER, "2")))))),
          make_unique<CompoundStmt>(
              Token(TokenType::BRACE_OPEN),
              Utils::vector<StatementListType>(make_unique<ExpressionStatement>(
                  Token(), make_unique<Binary>(
                               Token(TokenType::PLUS_ASSIGN),
                               make_unique<VariableName>(
                                   Token(TokenType::IDENTIFIER, "b")),
                               make_unique<Number>(
                                   Token(TokenType::NUMBER, "2")))))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\tif ((a == 1)) {\n"
                                   "\t\t(b += 2);\n"
                                   "\t} else {\n"
                                   "\t\t(b += 2);\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("pretty print if else inline") {
  auto root = make_unique<CompoundStmt>(
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
              make_unique<Binary>(
                  Token(TokenType::DIV),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"a")), make_unique<Number>(Token(TokenType::NUMBER, "3")))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\tif ((a == 1))\n"
                                   "\t\t(b += 2);\n"
                                   "\telse\n"
                                   "\t\t(a / 3);\n"
                                   "}\n"));
}

TEST_CASE("pretty print if else if else inline") {
  auto root = make_unique<CompoundStmt>(
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
                  make_unique<VariableName>(Token(TokenType::NUMBER, "1"))),
              make_unique<CompoundStmt>(
                  Token(),
Utils::vector<StatementListType>(make_unique<ReturnStatement>( Token(),
make_unique<VariableName>(Token( TokenType::NUMBER, "0")))))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\tif (1)\n"
                                   "\t\treturn (1 + 3);\n"
                                   "\telse if (0)\n"
                                   "\t\treturn 1;\n"
                                   "\telse {\n"
                                   "\t\treturn 0;\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("pretty print if else if else") {
  auto root = make_unique<CompoundStmt>(
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
                      make_unique<VariableName>(Token(TokenType::NUMBER, "1"))),
                  make_unique<ReturnStatement>(
                      Token(), make_unique<VariableName>(
                                   Token(TokenType::NUMBER, "0"))))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\tif (1)\n"
                                   "\t\treturn (1 + 3);\n"
                                   "\telse if (0) {\n"
                                   "\t\treturn 1;\n"
                                   "\t} else if (0)\n"
                                   "\t\treturn 1;\n"
                                   "\telse\n"
                                   "\t\treturn 0;\n"
                                   "}\n"));
}

TEST_CASE("pretty print if else if") {
  auto root = make_unique<CompoundStmt>(
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
make_unique<VariableName>(Token( TokenType::NUMBER, "1")))))))));

  REQUIRE(compare(std::move(root), "{\n"
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
      Utils::vector<StatementListType>(make_unique<WhileStatement>(
          Token(),
          make_unique<Binary>(
              Token(TokenType::PLUS),
              make_unique<VariableName>(Token(TokenType::NUMBER, "1")),
              make_unique<VariableName>(Token(TokenType::NUMBER, "3"))),
          make_unique<CompoundStmt>(
              Token(),
              Utils::vector<StatementListType>(make_unique<ExpressionStatement>(
                         Token(), make_unique<Binary>(
                                      Token(TokenType::PLUS),
                                      make_unique<Number>(
                                          Token(TokenType::NUMBER, "1")),
                                      make_unique<Number>(
                                          Token(TokenType::NUMBER, "3")))),
                     make_unique<ExpressionStatement>(
                         Token(), make_unique<Binary>(
                                      Token(TokenType::STAR),
                                      make_unique<Number>(
                                          Token(TokenType::NUMBER, "0")),
                                      make_unique<Number>(
                                          Token(TokenType::NUMBER, "5")))))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\twhile ((1 + 3)) {\n"
                                   "\t\t(1 + 3);\n"
                                   "\t\t(0 * 5);\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("pretty print while inline") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<WhileStatement>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "3")),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<ReturnStatement>(
                  Token(),
                  make_unique<VariableName>(Token(TokenType::NUMBER, "1"))),
              make_unique<ReturnStatement>(
                  Token(), make_unique<StringLiteral>(
                               Token(TokenType::STRING, "\"test\"")))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\twhile (3)\n"
                                   "\t\tif (1)\n"
                                   "\t\t\treturn 1;\n"
                                   "\t\telse\n"
                                   "\t\t\treturn \"test\";\n"
                                   "}\n"));
}

TEST_CASE("pretty print while inline if else break continue") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<WhileStatement>(
          Token(), make_unique<Number>(Token(TokenType::NUMBER, "3")),
          make_unique<IfElse>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<BreakStatement>(Token()),
              make_unique<ContinueStatement>(Token())))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\twhile (3)\n"
                                   "\t\tif (1)\n"
                                   "\t\t\tbreak;\n"
                                   "\t\telse\n"
                                   "\t\t\tcontinue;\n"
                                   "}\n"));
}

TEST_CASE("pretty print while inline break continue") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<WhileStatement>(
                 Token(), make_unique<Number>(Token(TokenType::NUMBER, "3")),
                 make_unique<BreakStatement>(Token())),
             make_unique<WhileStatement>(
                 Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
                 make_unique<ContinueStatement>(Token()))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\twhile (3)\n"
                                   "\t\tbreak;\n"
                                   "\twhile (1)\n"
                                   "\t\tcontinue;\n"
                                   "}\n"));
}

TEST_CASE("conditional") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<ExpressionStatement>(
          Token(),
          make_unique<ConditionalExpression>(
              Token(),
              make_unique<Binary>(
                  Token(TokenType::LESS),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"a")), make_unique<Number>(Token(TokenType::NUMBER, "0"))),
              make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<Number>(Token(TokenType::NUMBER, "3"))))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\t((a < 0) ? 1 : 3);\n"
                                   "}\n"));
}

TEST_CASE("pretty print while inline break continue blocks") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(
          make_unique<WhileStatement>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "3")),
              make_unique<CompoundStmt>(
                  Token(),
Utils::vector<StatementListType>(make_unique<BreakStatement>(Token())))),
          make_unique<WhileStatement>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "2")),
              make_unique<CompoundStmt>(
                  Token(),
                  Utils::vector<StatementListType>(make_unique<BreakStatement>(Token()),
                         make_unique<CompoundStmt>(
                             Token(),
Utils::vector<StatementListType>(make_unique<ContinueStatement>( Token())))))),
          make_unique<WhileStatement>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<CompoundStmt>(
                  Token(),
Utils::vector<StatementListType>(make_unique<ContinueStatement>(Token()))))));

  REQUIRE(compare(std::move(root), "{\n"
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
      Token(),
      Utils::vector<StatementListType>(
          make_unique<WhileStatement>(
              Token(), make_unique<Number>(Token(TokenType::NUMBER, "1")),
              make_unique<LabeledStatement>(
                  Token(),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"foo")), make_unique<BreakStatement>(Token()))), make_unique<GotoStatement>(
              Token(),
              make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"foo")))));

  REQUIRE(compare(std::move(root), "{\n"
                                   "\twhile (1)\n"
                                   "foo:\n"
                                   "\t\tbreak;\n"
                                   "\tgoto foo;\n"
                                   "}\n"));
}
TEST_CASE("pretty print if else if else goto label") {
  auto root = make_unique<CompoundStmt>(
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

  REQUIRE(compare(std::move(root), "{\n"
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
}

TEST_CASE("declaration") {
  std::vector<std::unique_ptr<TypeExpression>> tmp0;
  tmp0.push_back(std::move(make_unique<TypeExpression>(TypeSpec::INT)));
  std::vector<std::unique_ptr<TypeExpression>> tmp1;
  tmp1.push_back(std::move(make_unique<TypeExpression>(TypeSpec::VOID)));
  std::vector<std::unique_ptr<TypeExpression>> tmp2;
  tmp2.push_back(std::move(make_unique<TypeExpression>(TypeSpec::CHAR)));
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(
          make_unique<DeclarationStatement>(
              Token(), make_unique<TypeExpression>(
                           TypeSpec::INT, make_unique<VariableName>(Token(
                                              TokenType::IDENTIFIER, "a")))),
          make_unique<DeclarationStatement>(
              Token(),
              make_unique<TypeExpression>(
                  TypeSpec::INT,
                  make_unique<PointerTypeExpression>(make_unique<VariableName>(
                      Token(TokenType::IDENTIFIER, "b"))))),
          make_unique<DeclarationStatement>(
              Token(),
              make_unique<TypeExpression>(
                  TypeSpec::VOID, make_unique<PointerTypeExpression>(
                                      make_unique<PointerTypeExpression>(
                                          make_unique<VariableName>(Token(
                                              TokenType::IDENTIFIER,
"c")))))), make_unique<DeclarationStatement>( Token(),
make_unique<TypeExpression>( TypeSpec::VOID, make_unique<PointerTypeExpression>(
                               make_unique<FunctionTypeExpression>(
                                   make_unique<PointerTypeExpression>(
                                       make_unique<PointerTypeExpression>(
                                           make_unique<VariableName>(Token(
                                               TokenType::IDENTIFIER, "d")))),
                                   std::move(tmp0))))),
          make_unique<DeclarationStatement>(
              Token(),
              make_unique<TypeExpression>(
                  TypeSpec::INT, make_unique<FunctionTypeExpression>(
                                     make_unique<VariableName>(
                                         Token(TokenType::IDENTIFIER,
"main")), std::move(tmp1)))), make_unique<DeclarationStatement>( Token(),
              make_unique<StructTypeExpression>(
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"S")), make_unique<PointerTypeExpression>(make_unique<VariableName>(
                      Token(TokenType::IDENTIFIER, "p"))))),
          make_unique<DeclarationStatement>(
              Token(),
              make_unique<TypeExpression>(
                  TypeSpec::INT, make_unique<PointerTypeExpression>(
                                     make_unique<FunctionTypeExpression>(
                                         make_unique<VariableName>(Token(
                                             TokenType::IDENTIFIER, "test")),
                                         std::move(tmp2)))))));
  REQUIRE(compare(std::move(root), "{\n"
                                   "\tint a;\n"
                                   "\tint (*b);\n"
                                   "\tvoid (*(*c));\n"
                                   "\tvoid (*((*(*d))(int)));\n"
                                   "\tint (main(void));\n"
                                   "\tstruct S (*p);\n"
                                   "\tint (*(test(char)));\n"
                                   "}\n"));
}

TEST_CASE("declaration init") {
  std::vector<std::unique_ptr<TypeExpression>> tmp;
  tmp.push_back(std::move(make_unique<TypeExpression>(TypeSpec::VOID)));
  tmp.push_back(std::move(make_unique<TypeExpression>(TypeSpec::INT)));
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<DeclarationStatement>(
          Token(),
          make_unique<TypeExpression>(
              TypeSpec::INT,
              make_unique<FunctionTypeExpression>(
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"main")), std::move(tmp))), make_unique<CompoundStmt>( Token(),
Utils::vector<StatementListType>(make_unique<DeclarationStatement>( Token(),
make_unique<TypeExpression>( TypeSpec::INT, make_unique<VariableName>(Token(
                                            TokenType::IDENTIFIER, "d"))))))))

  );
  REQUIRE(compare(std::move(root), "{\n"
                                   "\tint (main(void, int))\n"
                                   "\t{\n"
                                   "\t\tint d;\n"
                                   "\t}\n"
                                   "}\n"));
}

TEST_CASE("sizeof") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(
          make_unique<ExpressionStatement>(
              Token(), make_unique<SizeOfExpression>(
                           Token(), make_unique<VariableName>(
                                        Token(TokenType::IDENTIFIER, "b")))),
          make_unique<ExpressionStatement>(
              Token(),
              make_unique<SizeOfExpression>(
                  Token(), make_unique<TypeExpression>(TypeSpec::INT))),
          make_unique<ExpressionStatement>(
              Token(),
              make_unique<SizeOfExpression>(
                  Token(),
                  make_unique<Binary>(
                      Token(TokenType::PLUS),
                      make_unique<Number>(Token(TokenType::NUMBER, "0")),
                      make_unique<Number>(Token(TokenType::NUMBER, "0")))))));
  REQUIRE(compare(std::move(root), "{\n"
                                   "\t(sizeof b);\n"
                                   "\t(sizeof(int));\n"
                                   "\t(sizeof (0 + 0));\n"
                                   "}\n"));
}

TEST_CASE("struct") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(make_unique<StructStatement>(
          Token(), make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"S")), make_unique<CompoundStmt>( Token(),
Utils::vector<StatementListType>(make_unique<DeclarationStatement>( Token(),
make_unique<TypeExpression>( TypeSpec::INT, make_unique<VariableName>(Token(
                                            TokenType::IDENTIFIER, "x")))))),
          make_unique<VariableName>(Token(TokenType::IDENTIFIER, "s")))));
  REQUIRE(compare(std::move(root), "{\n"
                                   "\tstruct S\n"
                                   "\t{\n"
                                   "\t\tint x;\n"
                                   "\t} s;\n"
                                   "}\n"));
}

TEST_CASE("postfix") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(
          make_unique<ExpressionStatement>(
              Token(),
              make_unique<PostfixExpression>(
                  Token(TokenType::DOT),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"s")), make_unique<VariableName>(Token(TokenType::IDENTIFIER, "x")))),
          make_unique<ExpressionStatement>(
              Token(),
              make_unique<PostfixExpression>(
                  Token(TokenType::ARROW),
                  make_unique<PostfixExpression>(
                      Token(TokenType::DOT),
                      make_unique<PostfixExpression>(
                          Token(TokenType::ARROW),
                          make_unique<VariableName>(
                              Token(TokenType::IDENTIFIER, "a")),
                          make_unique<VariableName>(
                              Token(TokenType::IDENTIFIER, "s"))),
                      make_unique<VariableName>(
                          Token(TokenType::IDENTIFIER, "x"))),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"b")))), make_unique<ExpressionStatement>( Token(), make_unique<Binary>(
                           Token(TokenType::ASSIGN),
                           make_unique<PostfixExpression>(
                               Token(TokenType::DOT),
                               make_unique<VariableName>(
                                   Token(TokenType::IDENTIFIER, "s")),
                               make_unique<VariableName>(
                                   Token(TokenType::IDENTIFIER, "x"))),
                           make_unique<VariableName>(
                               Token(TokenType::IDENTIFIER, "x"))))));
  REQUIRE(compare(std::move(root), "{\n"
                                   "\t(s.x);\n"
                                   "\t(((a->s).x)->b);\n"
                                   "\t((s.x) = x);\n"
                                   "}\n"));
}

TEST_CASE("unary") {
  auto root = make_unique<CompoundStmt>(
      Token(),
      Utils::vector<StatementListType>(
          make_unique<ExpressionStatement>(
              Token(),
              make_unique<UnaryExpression>(
                  Token(TokenType::AMPERSAND),
                  make_unique<VariableName>(Token(TokenType::IDENTIFIER,
"s")))), make_unique<ReturnStatement>( Token(), make_unique<UnaryExpression>(
                           Token(TokenType::MINUS),
                           make_unique<UnaryExpression>(
                               Token(TokenType::STAR),
                               make_unique<UnaryExpression>(
                                   Token(TokenType::AMPERSAND),
                                   make_unique<PostfixExpression>(
                                       Token(TokenType::DOT),
                                       make_unique<VariableName>(
                                           Token(TokenType::IDENTIFIER, "s")),
                                       make_unique<VariableName>(Token(
                                           TokenType::IDENTIFIER,
"x"))))))))); REQUIRE(compare(std::move(root), "{\n"
                                   "\t(&s);\n"
                                   "\treturn (-(*(&(s.x))));\n"
                                   "}\n"));
}

TEST_CASE("call") {
  auto root = new CompoundStmt(
      Token(),
      {
          new ExpressionStatement(
              Token(),
              new CallExpression(
                  Token(),
                  new VariableNameExpression(Token(TokenType::IDENTIFIER,
"f")), {new UnaryExpression(Token(TokenType::MINUS), new VariableNameExpression(
                                           Token(TokenType::IDENTIFIER,
"s"))), new VariableNameExpression(Token(TokenType::NUMBER, "1"))})),
      });
  REQUIRE(compare(root, "{\n"
                        "\t(f((-s), 1));\n"
                        "}\n"));
  delete root;
}
 */
} // namespace ccc
