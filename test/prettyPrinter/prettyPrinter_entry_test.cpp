#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"
#include "utils/utils.hpp"
#include <fstream>

using namespace ccc;
TEST_CASE("entry handler") {
  std::string flag = "--print-ast";
  std::string input = "../examples/sample.c";

  //  std::ifstream ifs("../" + input);
  //  std::stringstream buffer;
  //  buffer << ifs.rdbuf();
  //  std::string expected = buffer.str();
  std::string expected = "{\n}\n";

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

  std::vector<std::string> expected_lines = Utils::split_lines(expected);
  std::vector<std::string> content_lines = Utils::split_lines(content);

  for (unsigned long i = 0, errors = 0;
       i < std::max(content_lines.size(), expected_lines.size()); i++) {
    if (i >= expected_lines.size()) {
      std::cerr << "expected nothing but got \"" << content_lines[i] << "\""
                << std::endl;
    } else if (i >= content_lines.size()) {
      std::cerr << "expected \"" << expected_lines[i] << "\" but got nothing "
                << std::endl;
    } else if (content_lines[i] != expected_lines[i]) {
      std::cerr << "expected \"" << expected_lines[i] << "\" but got \""
                << content_lines[i] << "\"" << std::endl;
    } else {
      continue;
    }
    errors++;
    if (errors > 5) {
      std::cerr << std::endl
                << "Output truncated after five errors... - fix your stuff!"
                << std::endl;
      break;
    }
  }
  REQUIRE(expected == content);
}
