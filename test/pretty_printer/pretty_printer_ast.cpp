#include "../catch.hpp"
#include "ast/visitor/graphviz.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>

namespace ccc {

TEST_CASE("gv ast") {
  std::string language = "int *f(int);\n"
                         "int (*g)(int);\n";

  auto fp = FastParser(language);
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto root = fp.parse();
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int (*(f(int)));\n"
                                                  "\n"
                                                  "int ((*g)(int));\n"));
}

} // namespace ccc
