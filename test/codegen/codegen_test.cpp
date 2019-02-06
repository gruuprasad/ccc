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

#define REQUIRE_BUILD                                                          \
  FastParser fp = FastParser(input);                                           \
  auto root = fp.parse();                                                      \
  REQUIRE_SUCCESS(fp);                                                         \
  SemanticVisitor sv;                                                          \
  root->accept(&sv);                                                           \
  REQUIRE_SUCCESS(sv);                                                         \
  CodegenVisitor cv("test.c");                                                 \
  root->accept(&cv);                                                           \
  cv.dump();                                                                   \
  cv.compile();                                                                \
  system("../../llvm/install/bin/clang -w -o test test.ll");

#define REQUIRE_RUN(args, ret)                                                 \
  std::string cmd = "./test ";                                                 \
  cmd.append(args);                                                            \
  int run = system(&cmd[0]);                                                   \
  REQUIRE(WEXITSTATUS(run) == ret);

#define CLANG                                                                  \
  std::ofstream os("tmp.c");                                                   \
  os << input;                                                                 \
  os.close();                                                                  \
  system("../../llvm/install/bin/clang -w -emit-llvm -c -S -o tmp.ll tmp.c");  \
  std::ifstream is("tmp.ll");                                                  \
  std::string line;                                                            \
  if (is.is_open()) {                                                          \
    while (getline(is, line)) {                                                \
      std::cout << line << '\n';                                               \
    }                                                                          \
    is.close();                                                                \
  }

namespace ccc {
//
// TEST_CASE("ast codegen smoke test") {
//  PRINT_START("main");
//  std::string input = "int main() {\n"
//                      "  return 0 ? main() : 0;\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 0);
//}
//
// TEST_CASE("fac rec") {
//  PRINT_START("fac rec");
//  std::string input = "int fac(int);"
//                      "int main() {\n"
//                      "return fac(4);\n"
//                      "}\n"
//                      "int fac(int a) {\n"
//                      "  if(a < 1)\n"
//                      "    return 1;\n"
//                      "  else\n"
//                      "    return a * fac (a - 1);"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 24);
//}
//
// TEST_CASE("fac loop") {
//  PRINT_START("fac loop");
//  std::string input = "int fac(int);"
//                      "int main() {\n"
//                      "return fac(4);\n"
//                      "}\n"
//                      "int fac(int a) {\n"
//                      "  int b;\n"
//                      "  b = 1;\n"
//                      "  while(0 < a) {\n"
//                      "    b = b * a;\n"
//                      "    a = a - 1;\n"
//                      "  }\n"
//                      "  return b;\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 24);
//}
//
// TEST_CASE("fib rec") {
//  PRINT_START("fib rec");
//  std::string input = "int fib(int);"
//                      "int main() {\n"
//                      "return fib(11);\n"
//                      "}\n"
//                      "int fib(int a) {\n"
//                      "  if(a < 1)\n"
//                      "    return 0;\n"
//                      "  if(a == 1)\n"
//                      "    return 1;\n"
//                      "  return fib (a - 1) + fib (a - 2);"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 89);
//}
//
// TEST_CASE("fib loop") {
//  PRINT_START("fib loop");
//  std::string input = "void fib(int);"
//                      "int res;\n"
//                      "int main() {\n"
//                      "  fib(11);"
//                      "  return res;\n"
//                      "}\n"
//                      "void fib(int a) {\n"
//                      "  int i;\n"
//                      "  int f0;\n"
//                      "  int f1;\n"
//                      "  i = 0;\n"
//                      "  f0 = 0;\n"
//                      "  f1 = 1;\n"
//                      "  if (a < 1)\n"
//                      "    res = f0;\n"
//                      "  if (a == 1)\n"
//                      "    res = f1;\n"
//                      "  while (i + 1 < a) {\n"
//                      "    res = f0 + f1;\n"
//                      "    f0 = f1;\n"
//                      "    f1 = res;\n"
//                      "    i = i + 1;\n"
//                      "  }\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 89);
//}

// TEST_CASE("sizeof int") {
//  PRINT_START("sizeof int");
//  std::string input = "int main() {\n"
//                      "  return sizeof(sizeof(int));\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 4);
//}
//
// TEST_CASE("sizeof char") {
//  PRINT_START("sizeof char");
//  std::string input = "int main() {\n"
//                      "  return sizeof(char);\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 1);
//}
//
// TEST_CASE("sizeof char*") {
//  PRINT_START("sizeof char*");
//  std::string input = "int main() {\n"
//                      "  return sizeof(char*);\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 8);
//}
//
// TEST_CASE("sizeof string") {
//  PRINT_START("sizeof string");
//  std::string input = "int main() {\n"
//                      "  return sizeof(\"hello world\");\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 11);
//}
//
// TEST_CASE("sizeof func ptr") {
//  PRINT_START("sizeof string");
//  std::string input = "void (*foo)(void);"
//                      "int main() {\n"
//                      "  return sizeof(foo);\n"
//                      "}\n";
//  REQUIRE_BUILD_RUN(input, 8);
//}

TEST_CASE("int ptr") {
  PRINT_START("int ptr");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int a;"
                      "  a = 2;"
                      "  int* p;\n"
                      "  p = malloc(sizeof(a));"
                      "  *p = a;"
                      "  a = *p;"
                      "  return *p + a + 2;"
                      "}\n";
  CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 6);
}

TEST_CASE("int ptr 2") {
  PRINT_START("int ptr");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int a;"
                      "  a = 7;"
                      "  int*** q;\n"
                      "  q = malloc(sizeof(int***));"
                      "  *q = malloc(sizeof(int**));"
                      "  **q = malloc(sizeof(int));"
                      "  ***q = a;"
                      "  a = ***q;"
                      " return ***q;"
                      "}\n";
  CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 7);
}

TEST_CASE("main argc") {
  PRINT_START("int ptr");
  std::string input = "int printf(char *format, char*);"
                      ""
                      "int main(int argc, char **argv) {\n"
                      "   return argc;\n"
                      "}";
  CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("hallo", 2);
}

TEST_CASE("array test") {
  PRINT_START("int ptr");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int *a;"
                      "  a = malloc(sizeof(int) * 2);"
                      "  a[0] = 10;"
                      " return a[0];"
                      "}\n";
  CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 10);
}

TEST_CASE("array test 2") {
  PRINT_START("int ptr");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int **a;"
                      "  a = malloc(sizeof(int*) * 1);"
                      "  *a = malloc(sizeof(int) * 2);"
                      "  a[0][1] = 10;"
                      " return a[0][1];"
                      "}\n";
  CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 10);
}

} // namespace ccc
