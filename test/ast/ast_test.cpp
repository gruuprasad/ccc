#include "../catch.hpp"
#include "ast/visitor/graphviz.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>

namespace ccc {

TEST_CASE("gv ast") {
  std::string language = "struct A;\n"
                         "struct A a;\n"
                         "\n"
                         "void main () {\n"
                         "  struct A *p;\n"
                         "}\n"
                         "\n"
                         "int;"
                         "struct A;";

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
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
