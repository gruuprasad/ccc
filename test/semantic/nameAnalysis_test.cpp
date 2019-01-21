#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"
#include <iostream>

namespace ccc {

TEST_CASE("scope test") {
  std::string language = "int (f(int x, int y))\n"
                         "{\n"
                         "(1 + 2);\n"
                         "char (*a);\n"
                         "int b;\n"
                         "foo:\n"
                         "(1 ? b + 1 : a);\n"
                         "goto bar;\n"
                         "return;\n"
                         "int *c;\n"
                         "}\n";

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto gv = SemanticVisitor();
  root->accept(&gv);
  if (gv.fail())
    std::cerr << gv.getError() << std::endl;
}

} // namespace ccc
