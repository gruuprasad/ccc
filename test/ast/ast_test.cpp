#include "../catch.hpp"
#include "ast/graphviz.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>

namespace ccc {

TEST_CASE("gv ast") {
  std::string language = "int (f(int x, int y))\n"
                         "{\n"
                         "(1 + 2);"
                         "char (*a);\n"
                         "int b;\n"
                         "foo:\n"
                         "(1 ? b + 1 : 1);\n"
                         "goto bar;\n"
                         "bar:\n"
                         "return;\n"
                         "int *c;\n"
                         "}\n";

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  std::cout << root->prettyPrint(0) << std::endl;
  std::ofstream ofs;
  ofs.open("ast.gv");
  auto gv = GraphvizVisitor();
  ofs << root->graphviz(&gv) << std::endl;
  ofs.close();
  std::system("dot ast.gv -Tsvg > ast.svg");
}

} // namespace ccc
