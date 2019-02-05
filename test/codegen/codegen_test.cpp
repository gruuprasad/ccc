#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/codegen.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"

#define PRINT_START(n)                                                         \
  std::cout                                                                    \
      << "\033[33m"                                                            \
      << "===================================================================" \
         "============\n"                                                      \
      << n                                                                     \
      << "\n============================================================="     \
         "==================\033[0m"                                           \
      << std::endl;

#define REQUIRE_BUILD_RUN(input, ret)                                          \
  FastParser fp = FastParser(input);                                           \
  auto root = fp.parse();                                                      \
  REQUIRE_SUCCESS(fp);                                                         \
  SemanticVisitor sv;                                                          \
  root->accept(&sv);                                                           \
  REQUIRE_SUCCESS(sv);                                                         \
  CodegenVisitor cv;                                                           \
  root->accept(&cv);                                                           \
  cv.dump();                                                                   \
  int run =                                                                    \
      system("../../llvm/install/bin/clang -w -o test test.ll && ./test");     \
  REQUIRE(WEXITSTATUS(run) == ret);

namespace ccc {

TEST_CASE("ast codegen smoke test") {
  PRINT_START("main");
  std::string input = "int main() {\n"
                      "  return 0;\n"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 0);
}

TEST_CASE("fac rec") {
  PRINT_START("fac rec");
  std::string input = "int fac(int);"
                      "int main() {\n"
                      "return fac(4);\n"
                      "}\n"
                      "int fac(int a) {\n"
                      "  if(a < 1)\n"
                      "    return 1;\n"
                      "  else\n"
                      "    return a * fac (a - 1);"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 24);
}

TEST_CASE("fac loop") {
  PRINT_START("fac loop");
  std::string input = "int fac(int);"
                      "int main() {\n"
                      "return fac(4);\n"
                      "}\n"
                      "int fac(int a) {\n"
                      "  int b;\n"
                      "  b = 1;\n"
                      "  while(0 < a) {\n"
                      "    b = b * a;\n"
                      "    a = a - 1;\n"
                      "  }\n"
                      "  return b;\n"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 24);
}

TEST_CASE("fib rec") {
  PRINT_START("fib rec");
  std::string input = "int fib(int);"
                      "int main() {\n"
                      "return fib(11);\n"
                      "}\n"
                      "int fib(int a) {\n"
                      "  if(a < 1)\n"
                      "    return 0;\n"
                      "  if(a == 1)\n"
                      "    return 1;\n"
                      "  return fib (a - 1) + fib (a - 2);"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 89);
}

TEST_CASE("fib loop") {
  PRINT_START("fib loop");
  std::string input = "void fib(int);"
                      "int res;\n"
                      "int main() {\n"
                      "  fib(11);"
                      "  return res;\n"
                      "}\n"
                      "void fib(int a) {\n"
                      "  int i;\n"
                      "  int f0;\n"
                      "  int f1;\n"
                      "  i = 0;\n"
                      "  f0 = 0;\n"
                      "  f1 = 1;\n"
                      "  if (a < 1)\n"
                      "    res = f0;\n"
                      "  if (a == 1)\n"
                      "    res = f1;\n"
                      "  while (i + 1 < a) {\n"
                      "    res = f0 + f1;\n"
                      "    f0 = f1;\n"
                      "    f1 = res;\n"
                      "    i = i + 1;\n"
                      "  }\n"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 89);
}

} // namespace ccc
