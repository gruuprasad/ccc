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
  REQUIRE(WEXITSTATUS(system(                                                  \
              "../../llvm/install/bin/clang -w -o test test.ll && ./test")) == \
          ret);

namespace ccc {

TEST_CASE("ast codegen smoke test") {
  PRINT_START("main");
  std::string input = "int main() {\n"
                      "return 0;\n"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 0);
}

TEST_CASE("fac") {
  PRINT_START("fac");
  std::string input = "int fac(int a) {\n"
                      "if(a < 1)\n"
                      "return 1;\n"
                      "else\n"
                      "return a * fac (a - 1);"
                      "}\n"
                      "int main() {\n"
                      "return fac(4);\n"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 24);
}

TEST_CASE("fib") {
  PRINT_START("fib");
  std::string input = "int fib(int a) {\n"
                      "if(a < 1)\n"
                      "return 0;\n"
                      "if(a == 1)\n"
                      "return 1;\n"
                      "return fib (a - 1) + fib (a - 2);"
                      "}\n"
                      "int main() {\n"
                      "return fib(11);\n"
                      "}\n";
  REQUIRE_BUILD_RUN(input, 89);
}

} // namespace ccc
