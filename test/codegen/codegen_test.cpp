#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/codegen.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"

namespace ccc {

TEST_CASE("ast codegen smoke test") {
  auto root = make_unique<Number>(Token(), 1);
  CodegenVisitor cv;
  auto code = root->accept(&cv);
  REQUIRE(cv.print(code) == "i64 1");
  std::cout << cv.print(code) << std::endl;
  cv.dump("test.ll", code);
  system("../../llvm/install/bin/clang -o test test.ll && ./test);
}

} // namespace ccc
