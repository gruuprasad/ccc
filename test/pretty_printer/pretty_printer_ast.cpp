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

TEST_CASE("unary postfix") { // FIXME
  auto input = "int main() {\n"
               //               "s.a;\n"
               //               "*s.a;\n"
               //               "*s->a;\n"
               "s.a.b;\n"
               "*s.a.b;\n"
               "*s->a->b;\n"
               "}\n";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(
      root->accept(&pp),
      "int (main())\n"
      "{\n"
      //                                                  "\t(s.a);\n"
      //                                                  "\t(*(s.a));\n"
      //                                                  "\t(*(s->a));\n"
      "\t((s.a).b);\n"
      "\t(*((s.a).b));\n"
      "\t(*((s->a)->b));\n"
      "}\n"));
}

} // namespace ccc
