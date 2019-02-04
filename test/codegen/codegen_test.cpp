#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/codegen.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"

namespace ccc {

TEST_CASE("ast codegen smoke test") {
  std::string input = "int main(int a) {\n"
                      "return (0 ? 1 : 3);"
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
  int ret = system("../../llvm/install/bin/clang -w -o test test.ll && ./test");
  REQUIRE(WEXITSTATUS(ret) == 3);
}

} // namespace ccc
