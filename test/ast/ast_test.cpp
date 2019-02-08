#include "../catch.hpp"
#include "ast/visitor/graphviz.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"

namespace ccc {
TEST_CASE("gv ast") {
  std::string language = "int main() {"
                         "  int a;"
                         "  int b;"
                         "  a = 1;"
                         "  b = 1;"
                         "  a || (b = 0);"
                         "  return b;"
                         "}";

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  SemanticVisitor sv;
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
  sv.printScopes();
  PrettyPrinterVisitor pp;
  std::cout << root->accept(&pp) << std::endl;
  std::ofstream ofs;
  ofs.open("ast.gv");
  GraphvizVisitor gv;
  ofs << root->accept(&gv) << std::endl;
  ofs.close();
  std::system("dot ast.gv -Tsvg > ast.svg");
  std::system("firefox ast.svg");
}
} // namespace ccc
