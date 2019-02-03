#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/codegen.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"

namespace ccc {

TEST_CASE("ast codegen smoke test") {
  std::string input = "int main(int a) {\n"
                      "if (a + 0) {\n"
                      "}\n"
                      "else{\n"
                      "}\n"
                      "}\n";
  FastParser fp = FastParser(input);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  SemanticVisitor sv;
  root->accept(&sv);
  REQUIRE_SUCCESS(sv);
  CodegenVisitor cv;
  root->accept(&cv);
  cv.dump();
  //  system("../../llvm/install/bin/clang -o test test.ll && ./test");
}

} // namespace ccc
