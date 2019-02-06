#include "../catch.hpp"
#include "ast/visitor/graphviz.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>

namespace ccc {

TEST_CASE("gv ast") {
  std::string language = "int addInt(int n, int m) {\n"
                         "    return n + m;\n"
                         "}\n"
                         "\n"
                         "int (*functionFactory(char n))(int a, int b) {\n"
                         "    int (*functionPtr)(int,int);\n"
                         "    functionPtr = &addInt;\n"
                         "    return functionPtr;\n"
                         "}"
                         "void main() {"
                         "  (*(functionFactory(1)))(1, 2);"
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
