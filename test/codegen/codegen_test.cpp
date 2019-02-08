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

TEST_CASE("ast codegen smoke test") {
  PRINT_START("main");
  std::string input = "int main() {\n"
                      "  return 0 ? main() : 0;\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
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
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 24);
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
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 24);
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
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 89);
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
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 89);
}

TEST_CASE("sizeof int") {
  PRINT_START("sizeof int");
  std::string input = "int main() {\n"
                      "  return sizeof(sizeof(int));\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 8);
}

TEST_CASE("sizeof c") {
  PRINT_START("sizeof c");
  std::string input = "int main() {\n"
                      "  return sizeof('c');\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 4);
}

TEST_CASE("sizeof char") {
  PRINT_START("sizeof char");
  std::string input = "int main() {\n"
                      "  return sizeof(char);\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("sizeof char*") {
  PRINT_START("sizeof char*");
  std::string input = "int main() {\n"
                      "  return sizeof(char*);\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 8);
}

TEST_CASE("sizeof string") {
  PRINT_START("sizeof string");
  std::string input = "int main() {\n"
                      "  return sizeof(\"hello world\");\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 12);
}

TEST_CASE("sizeof func ptr") {
  PRINT_START("sizeof string");
  std::string input = "void (*foo)(void);"
                      "int main() {\n"
                      "  return sizeof(foo);\n"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 8);
}

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
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 6);
}

TEST_CASE("int ptr 2") {
  PRINT_START("int ptr 2");
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
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 7);
}

TEST_CASE("main argc") {
  PRINT_START("main argc");
  std::string input = "int printf(char *format, char*);"
                      ""
                      "int main(int argc, char **argv) {\n"
                      "   return argc;\n"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("hallo", 2);
}

TEST_CASE("array test") {
  PRINT_START("array test");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int *a;"
                      "  a = malloc(sizeof(int) * 2);"
                      "  a[0] = 10;"
                      " return a[0];"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 10);
}

TEST_CASE("array test 2") {
  PRINT_START("array test 2");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int **a;"
                      "  a = malloc(sizeof(int*) * 1);"
                      "  *a = malloc(sizeof(int) * 2);"
                      "  a[0][1] = 10;"
                      " return a[0][1];"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 10);
}

TEST_CASE("ptr add test") {
  PRINT_START("ptr add test");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int *a;"
                      "  a = malloc(sizeof(int) * 3);"
                      "  a[0] = 10;"
                      "  *(a + 1) = a[0] + 2;"
                      "  *(a + 2) = *(1 + a);"
                      "  return *(2 + a);"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 12);
}

TEST_CASE("ptr diff") {
  PRINT_START("ptr diff");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {\n"
                      "  int *a;"
                      "  int *b;"
                      "  return a - b;"
                      "}\n";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
}

TEST_CASE("array loop") {
  PRINT_START("array loop");
  std::string input = "void *malloc(int);"
                      ""
                      "int main(int argc, char **argv) {"
                      "   int i;"
                      "   i = 0;"
                      "   int *a;"
                      "   a = malloc(sizeof(int) * argc);"
                      "   while (i < argc) {"
                      "     a[i] = i;"
                      "     i = i + 1;"
                      "   }"
                      "   return a[argc - 1] == a[argc - 2] + 1;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("a b c d e", 1);
}

TEST_CASE("argv access") {
  PRINT_START("argv access");
  std::string input = "int main(int argc, char **argv) {"
                      "   return argv[1][0];"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("a", 97);
}

TEST_CASE("string") {
  PRINT_START("string");
  std::string input = "int main() {"
                      "   char * s;"
                      "   s = \"abc\";"
                      "   return  s[2];"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 99);
}

TEST_CASE("cast") {
  PRINT_START("cast");
  std::string input = "int main() {"
                      "   int a;"
                      "   char b;"
                      "   a = 'c';"
                      "   b = 1;"
                      "   return  a + b;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 100);
}

TEST_CASE("cast 2") {
  PRINT_START("cast 2");
  std::string input = "int main() {"
                      "   return  (1 < 3) + 'b';"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 99);
}

TEST_CASE("cast 3") {
  PRINT_START("cast 3");
  std::string input = "char main() {"
                      "   return  1;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("cast 4") {
  PRINT_START("cast 4");
  std::string input = "void main() {"
                      "   int *p;"
                      "   p = 0;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
}

TEST_CASE("return void") {
  PRINT_START("return void");
  std::string input = "void main() {"
                      "   return;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
}

TEST_CASE("return ternary") {
  PRINT_START("return ternary");
  std::string input = "int main() {"
                      "   char a;"
                      "   a = 'a';"
                      "   return 0 ? 3 : a;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 97);
}

TEST_CASE("ptr ternary") {
  PRINT_START("ptr ternary");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {"
                      "   int *a;"
                      "   a = malloc(sizeof(int));"
                      "   *a = a ? 0 : 1;"
                      "   return *a;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
}

TEST_CASE("ptr ternary 2") {
  PRINT_START("ptr ternary 2");
  std::string input = "int main() {"
                      "   int *a;"
                      "   a = 0;"
                      "   return a ? 0 : 1;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("ptr comp") {
  PRINT_START("ptr comp");
  std::string input = "int main() {"
                      "   int *a;"
                      "   return (a == 0 && a != 0) || a == a;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("ptr deref") {
  PRINT_START("ptr deref");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {"
                      "  int *a;"
                      "  int b;"
                      "  b = 5;"
                      "  a = malloc(sizeof(int));"
                      "  a = &b;"
                      "  return a[0];"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 5);
}

TEST_CASE("not") {
  PRINT_START("not");
  std::string input = "int main() {"
                      "  return !(2 < 1);"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("minus") {
  PRINT_START("minus");
  std::string input = "int main() {"
                      "  return -5 + 6;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("ptr not") {
  PRINT_START("ptr not");
  std::string input = "int main() {"
                      "  int *a;"
                      "  a = 0;"
                      "  return !a;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("args ptr") {
  PRINT_START("args ptr");
  std::string input = "int puts(char *str);"
                      ""
                      "int main() {"
                      "  char *s;"
                      "  s = \"test\";"
                      "  puts(s);"
                      "  return 0;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
}

TEST_CASE("puts") {
  PRINT_START("puts");
  std::string input = "int puts(char *str);"
                      ""
                      "int main() {"
                      "  char *s;"
                      "  s = \"test\";"
                      "  puts(s);"
                      "  return 0;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 0);
}

TEST_CASE("extern vars") {
  PRINT_START("extern vars");
  std::string input = "void *malloc(int);"
                      ""
                      "int a;"
                      "int a;"
                      "int *b;"
                      "int *b;"
                      "char *s;"
                      ""
                      "int main() {"
                      "  s = \"test\";"
                      "  a = 1;"
                      "  b = malloc(sizeof(int));"
                      "  *b = 1;"
                      "  return a + *b;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 2);
}

TEST_CASE("lazy or") {
  PRINT_START("lazy or");
  std::string input = "int main() {"
                      "  int a;"
                      "  int b;"
                      "  a = 0;"
                      "  b = 1;"
                      "  a || (b = 1) || (a = 1);"
                      "  return a + b;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("lazy and") {
  PRINT_START("lazy and");
  std::string input = "int main() {"
                      "  int a;"
                      "  int b;"
                      "  a = 1;"
                      "  b = 1;"
                      "  a && (b = 0) && (a = 0);"
                      "  return a + b;"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("ptr arr") {
  PRINT_START("ptr arr");
  std::string input = "void *malloc(int);"
                      ""
                      "int main() {"
                      "  int *dr;\n"
                      "  int *aa;\n"
                      "  aa = malloc(5 * sizeof(int));\n"
                      "  dr = &aa[3];\n"
                      "  aa[0] = 0;"
                      "  aa[1] = 1;"
                      "  aa[2] = 2;"
                      "  aa[3] = 3;"
                      "  return *(dr - 2);"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 1);
}

TEST_CASE("struct size") {
  PRINT_START("struct size");
  std::string input = "int main() {"
                      "  struct S { char y; int a; int b; int c; } s;"
                      "  return sizeof(s);"
                      "}";
  // CLANG;
  REQUIRE_BUILD;
  REQUIRE_RUN("", 16);
}

} // namespace ccc
