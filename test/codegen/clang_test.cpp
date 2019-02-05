#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include "ast/visitor/codegen.hpp"
#include "ast/visitor/semantic_analysis.hpp"
#include "parser/fast_parser.hpp"

#include <fstream>
#include <iostream>

namespace ccc {

TEST_CASE("ast codegen smoke test") {
  std::string input = "int i;"
                      "int main() {\n"
                      "i = 0;"
                      "}\n";

  std::ofstream os("tmp.c");
  os << input;
  os.close();
  system("../../llvm/install/bin/clang -w -emit-llvm -c -S -o tmp.ll tmp.c");
  std::ifstream is("tmp.ll");
  std::string line;
  if (is.is_open()) {
    while (getline(is, line)) {
      std::cout << line << '\n';
    }
    is.close();
  }
}

} // namespace ccc
