
#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include "utils/utils.hpp"
#include <fstream>

using namespace ccc;
TEST_CASE("entry handler") {
  std::string flag = "--print-ast";
  std::string input = "../../examples/sample.c";

  std::ifstream ifs(input);
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  std::string expected = buffer.str();

  std::stringstream ss;
  std::streambuf *sb = std::cout.rdbuf();
  std::cout.rdbuf(ss.rdbuf());

  char **ppArgs = new char *[3];
  ppArgs[1] = &flag[0];
  ppArgs[2] = &input[0];

  REQUIRE(EXIT_SUCCESS == EntryPointHandler().handle(3, ppArgs));

  delete[] ppArgs;

  std::cout.rdbuf(sb);
  std::string content = ss.str();

  REQUIRE_EMPTY(Utils::compare(content, expected));
}
