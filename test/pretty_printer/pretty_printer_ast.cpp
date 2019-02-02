#include "../catch.hpp"
#include "ast/visitor/graphviz.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <utils/utils.hpp>

namespace ccc {

TEST_CASE("gv ast") {
  std::string language = "int *a(int);\n"
                         "int *(b(int));\n"
                         "int (*c(int));\n"
                         "int (*d)(int);\n"
                         "int ((*e)(int));\n"
                         "int (**e)(int);\n"
                         "\n";

  auto fp = FastParser(language);
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto root = fp.parse();
  auto pp = PrettyPrinterVisitor{};
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int (*(a(int)));\n"
                                                  "\n"
                                                  "int (*(b(int)));\n"
                                                  "\n"
                                                  "int (*(c(int)));\n"
                                                  "\n"
                                                  "int ((*d)(int));\n"
                                                  "\n"
                                                  "int ((*e)(int));\n"
                                                  "\n"
                                                  "int ((*(*e))(int));\n"));
}

TEST_CASE("unary postfix") { // FIXME
  std::string input = "int main() {\n"
                      "s.a;\n"
                      "*s.a;\n"
                      "*s->a;\n"
                      "s.a.b;\n"
                      "*s.a.b;\n"
                      "*s->a->b;\n"
                      "}\n";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int (main())\n"
                                                  "{\n"
                                                  "\t(s.a);\n"
                                                  "\t(*(s.a));\n"
                                                  "\t(*(s->a));\n"
                                                  "\t((s.a).b);\n"
                                                  "\t(*((s.a).b));\n"
                                                  "\t(*((s->a)->b));\n"
                                                  "}\n"));
}

TEST_CASE("sizeof abstract") {
  std::string input = "int main(){\n"
                      "sizeof(int *);\n"
                      "sizeof(void *);"
                      "sizeof(char **);\n"
                      "sizeof(char *(*));\n"
                      "sizeof(char (**));\n"
                      "sizeof(char (*(*)));\n"
                      "}\n";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int (main())\n"
                                                  "{\n"
                                                  "\t(sizeof(int (*)));\n"
                                                  "\t(sizeof(void (*)));\n"
                                                  "\t(sizeof(char (*(*))));\n"
                                                  "\t(sizeof(char (*(*))));\n"
                                                  "\t(sizeof(char (*(*))));\n"
                                                  "\t(sizeof(char (*(*))));\n"
                                                  "}\n"));
}

TEST_CASE("negative number") {
  std::string input = "int main() {\n"
                      "int a;"
                      "a = -5;"
                      "a - -5;"
                      "}\n";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int (main())\n"
                                                  "{\n"
                                                  "\tint a;\n"
                                                  "\t(a = -5);\n"
                                                  "\t(a - -5);\n"
                                                  "}\n"));
}

TEST_CASE("sizeof number") {
  std::string input = "int main() {\n"
                      "sizeof(5);\n"
                      "sizeof(-5);\n"
                      "sizeof(5 + 1);\n"
                      "sizeof(main);\n"
                      "sizeof(int);\n"
                      "sizeof(int**);\n"
                      "}\n";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp), "int (main())\n"
                                                  "{\n"
                                                  "\t(sizeof 5);\n"
                                                  "\t(sizeof -5);\n"
                                                  "\t(sizeof (5 + 1));\n"
                                                  "\t(sizeof main);\n"
                                                  "\t(sizeof(int));\n"
                                                  "\t(sizeof(int (*(*))));\n"
                                                  "}\n"));
}

TEST_CASE("precedence") {
  std::string input = "void main () {\n"
                      "int a;\n"
                      "int b;\n"
                      "int c;\n"
                      "int d;"
                      "c = d ? b : c;\n"
                      "c = d ? b : a ? c : d;\n"
                      "i = 5 || 2 ? a = 2 : b;"
                      "i = 5 || 2 ? a = 2 : d ? b : c;"
                      "1 + 1 * 1 * (1 * 1) - 1;\n"
                      "1 && 1 || 1 * 1 || 1 == 1;\n"
                      "a = b = 0;\n"
                      "}";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp),
                               "void (main())\n"
                               "{\n"
                               "\tint a;\n"
                               "\tint b;\n"
                               "\tint c;\n"
                               "\tint d;\n"
                               "\t(c = (d ? b : c));\n"
                               "\t(c = (d ? b : (a ? c : d)));\n"
                               "\t(i = ((5 || 2) ? (a = 2) : b));\n"
                               "\t(i = ((5 || 2) ? (a = 2) : (d ? b : c)));\n"
                               "\t((1 + ((1 * 1) * (1 * 1))) - 1);\n"
                               "\t(((1 && 1) || (1 * 1)) || (1 == 1));\n"
                               "\t(a = (b = 0));\n"
                               "}\n"));
}

TEST_CASE("super abstract") {
  std::string input = "void f(int *());"
                      "void f(int *(*()));";
  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  REQUIRE_EMPTY(Utils::compare(root->accept(&pp),
                               "void (f(int (*(()))));\n"
                               "\n"
                               "void (f(int (*(*(())))));\n"));
}

} // namespace ccc
