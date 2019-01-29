#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/pretty_printer.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"
#include <iostream>

namespace ccc {

TEST_CASE("duplicate label") {
  std::string input = "int main() {\n"
                      "foo:\n"
                      "foo:\n"
                      "  goto foo;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(3, 1, "Redefinition of label 'foo'"));
}

TEST_CASE("undeclared label") {
  std::string input = "int main() {\n"
                      "  goto foo;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() ==
          SEMANTIC_ERROR(2, 8, "Use of undeclared label 'foo'"));
}

TEST_CASE("double goto") {
  std::string input = "int main() {\n"
                      "   goto bar;\n"
                      "   goto bar;\n"
                      "bar:\n"
                      "  goto bar;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("cross label") {
  std::string input = "int main() {\n"
                      "foo:\n"
                      "   goto bar;\n"
                      "bar:\n"
                      "  goto foo;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("break & continue in loop") {
  std::string input = "int main() {\n"
                      "while(0)\n"
                      "  continue;\n"
                      "while(1)\n"
                      "  while(0)\n"
                      "    break;\n"
                      "while(0)\n"
                      "  continue;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("break outside loop") {
  std::string input = "int main() {\n"
                      "while(0)\n"
                      "  continue;\n"
                      "break;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(4, 1,
                                          "'break' statement not in a loop "
                                          "statement"));
}

TEST_CASE("continue outside loop") {
  std::string input = "int main() {\n"
                      "while(0)\n"
                      "  break;\n"
                      "continue;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() ==
          SEMANTIC_ERROR(4, 1, "'continue' statement not in a loop statement"));
}

TEST_CASE("method redefinition") {
  std::string input = "int main() {}\n"
                      "\n"
                      "int main() {}\n"
                      "\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(3, 5, "Redefinition of 'main'"));
}

TEST_CASE("duplicate int") {
  std::string input = "int a;\n"
                      "int a;\n"
                      "int main(int a) {\n"
                      "{int a;}\n"
                      "int a;}\n"
                      "\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(5, 5, "Redefinition of 'a'"));
}

TEST_CASE("incomplete method") {
  std::string input = "int f(int);\n"
                      "void main(int a) {\n"
                      "int b;"
                      "a = f(a);\n"
                      "}\n"
                      "int f(int a) {return a;}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("duplicate struct") {
  std::string input = "void main(int a) {\n"
                      "struct S {\n"
                      "int a;\n"
                      "} s;\n"
                      "struct S {\n"
                      "int a;\n"
                      "} t;\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(5, 8, "Redefinition of 'S'"));
}

TEST_CASE("duplicate field") {
  std::string input = "void main(int a) {\n"
                      "struct S {\n"
                      "int a;\n"
                      "int x;\n"
                      "int x;\n"
                      "};\n"
                      "}\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(5, 5, "Redefinition of 'x'"));
}

TEST_CASE("duplicate field nested struct") {
  std::string input = "struct S {\n"
                      " int a;\n"
                      " int x;\n"
                      " struct T {\n"
                      "   int* foo;\n"
                      "   int a;\n"
                      "   int x;\n"
                      "   struct S {\n"
                      "     int a;\n"
                      "     int x;\n"
                      "     int x;\n"
                      "   };\n"
                      " };\n"
                      "};\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() == SEMANTIC_ERROR(11, 10, "Redefinition of 'x'"));
}

TEST_CASE("redfinition struct name") {
  std::string input = "int foo () {\n"
                      "struct a{\n"
                      " int a;\n"
                      "};\n"
                      "int a;\n"
                      "}";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("anaonymous struct") {
  std::string input = "struct {};\n"
                      "struct S{\n"
                      " int a;\n"
                      " int x;\n"
                      "} s;\n"
                      "struct {\n"
                      " int a;\n"
                      " int x;\n"
                      "} t;\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("anaonymous struct redifinition") {
  std::string input = "struct {} s;\n"
                      "struct {\n"
                      " int a;\n"
                      " int x;\n"
                      "} s;\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() ==
          SEMANTIC_ERROR(5, 3, "Redefinition of 's' with a different type"));
}

TEST_CASE("anaonymous struct redifinition ugly") {
  std::string input = "struct {\n"
                      " int x;\n"
                      " int x;\n"
                      "} ;\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("struct nested same name") {
  std::string input = "struct x;\n"
                      "struct s;\n"
                      "struct x {\n"
                      "  struct a {\n"
                      "    struct x {};\n"
                      "  };\n"
                      "};\n"
                      "struct a {\n"
                      "  struct a {};\n"
                      "};";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_FAILURE(sv);
  REQUIRE(sv.getError() ==
          SEMANTIC_ERROR(9, 10, "Member 'a' has the same name as its class"));
}

TEST_CASE("use struct") {
  std::string input = "int foo() {\n"
                      "  struct S {};\n"
                      "  struct S *p;\n"
                      "}\n"
                      "struct S {} s;\n"
                      "struct S *p;\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
}

TEST_CASE("scoping") {
  std::string input = "int *a;\n"
                      "int foo (void, int) {\n"
                      "}\n"
                      "struct A *p;\n"
                      "int main () {\n"
                      "return foo(1, 2);\n"
                      "}\n"
                      "\n";

  auto fp = FastParser(input);
  auto root = fp.parse();
  if (fp.fail())
    std::cerr << fp.getError() << std::endl;
  auto pp = PrettyPrinterVisitor();
  std::cout << root->accept(&pp) << std::endl;
  auto sv = SemanticVisitor();
  root->accept(&sv);
  if (sv.fail())
    std::cerr << sv.getError() << std::endl;
  REQUIRE_SUCCESS(sv);
}

} // namespace ccc
