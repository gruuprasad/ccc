#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/pretty_printer.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"
#include <iostream>

namespace ccc {

TEST_CASE("duplicate label") {
  std::string input = "int main() {\n"
                      "foo:\n"
                      "foo:\n"
                      "  goto foo;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(3, 1, "Redefinition of label 'foo'"));
}

TEST_CASE("undeclared label") {
  std::string input = "int main() {\n"
                      "  goto foo;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() ==
          SEMANTIC_ERROR(2, 8, "Use of undeclared label 'foo'"));
}

TEST_CASE("cross label") {
  std::string input = "int main() {\n"
                      "foo:\n"
                      "   goto bar;\n"
                      "bar:\n"
                      "  goto foo;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("break & continue in loop") {
  std::string input = "int main() {\n"
                      "while(0)\n"
                      "  continue;\n"
                      "while(1)\n"
                      "  while(0)\n"
                      "    break;\n"
                      "while(0)\n"
                      "  continue;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("break outside loop") {
  std::string input = "int main() {\n"
                      "while(0)\n"
                      "  continue;\n"
                      "break;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(4, 1,
                                          "'break' statement not in a loop "
                                          "statement"));
}

TEST_CASE("continue outside loop") {
  std::string input = "int main() {\n"
                      "while(0)\n"
                      "  break;\n"
                      "continue;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() ==
          SEMANTIC_ERROR(4, 1, "'continue' statement not in a loop statement"));
}

TEST_CASE("method redefifinition") {
  std::string input = "int main() {}\n"
                      "int main() {}\n"
                      "\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
}

TEST_CASE("duplicate int") {
  std::string input = "int a;\n"
                      "int a;\n"
                      "int main(int a) {\n"
                      "{int a;}\n"
                      "int a;}\n"
                      "\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(5, 5, "Redefinition of 'a'"));
}

TEST_CASE("incomplete methode") {
  std::string input = "int f(int);\n"
                      "void main(int a) {\n"
                      "a = f(a);\n"
                      "}\n"
                      "int f(int a) {return a;}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("scoping") {
  std::string input = "int g;\n"
                      "int g;\n"
                      "int main(int g, int y);\n"
                      "int(*((**p)(int)));\n"
                      "int f(){main(5, 5);}\n"
                      "int main(int x, int y) {\n"
                      "int a;\n"
                      "{\n"
                      "int y;\n"
                      "}\n"
                      "y = main(5, 1) + 5;"
                      "}\n"
                      "int main(int, int) {}\n"
                      "\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  auto pp = PrettyPrinterVisitor();
  std::cout << root->accept(&pp) << std::endl;
  root->accept(&sv);
  if (sv.fail())
    std::cerr << sv.getError() << std::endl;
}

} // namespace ccc
