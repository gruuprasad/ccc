#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/semantic_analysis.hpp"
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
  REQUIRE(sv.getError() == SEMANTIC_ERROR(4, 1,
                                          "'continue' statement not in a loop "
                                          "statement"));
}

} // namespace ccc
