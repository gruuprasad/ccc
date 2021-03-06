#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "utils/utils.hpp"
#include <iterator>

using namespace ccc;
#define COMPARE(name)                                                          \
  TEST_CASE("Compare " #name ".c to " #name ".txt") {                          \
    REQUIRE(lexing_of(#name ".c", to_match(#name ".txt")));                    \
  }

bool lexing_of(const std::string &filename, const std::string &expected) {
  std::string flag = "--tokenize";
  std::string input = "../examples/" + filename;

  std::stringstream ss;
  std::streambuf *sb = std::cout.rdbuf();
  std::cout.rdbuf(ss.rdbuf());
  char **ppArgs = new char *[3];
  ppArgs[1] = &flag[0];
  ppArgs[2] = &input[0];
  REQUIRE(0 == EntryPointHandler().handle(3, ppArgs));
  delete[] ppArgs;
  std::cout.rdbuf(sb);
  std::vector<std::string> expected_lines = Utils::split_lines(expected);
  std::vector<std::string> content_lines = Utils::split_lines(ss.str());

  unsigned int counter = 0;
  for (unsigned long i = 0;
       i < std::max(content_lines.size(), expected_lines.size()); i++) {
    if (content_lines[i] == expected_lines[i]) {
      continue;
    }
    if (i >= expected_lines.size()) {
      std::cerr << filename << ": expected nothing but got \""
                << content_lines[i] << "\"." << std::endl;
    } else if (i >= content_lines.size()) {
      std::cerr << filename << ": expected \"" << expected_lines[i]
                << "\" but got nothing." << std::endl;
    } else if (content_lines[i] != expected_lines[i]) {
      std::cerr << filename << ": expected \"" << expected_lines[i]
                << "\" but got \"" << content_lines[i] << "\"." << std::endl;
    }
    counter += 1;
    if (counter > 5) {
      std::cerr << std::endl << "Output truncated after 5 lines...";
      return false;
    }
    return counter == 0;
  }
  return true;
}

std::string to_match(const std::string &filename) {
  std::ifstream t("../examples/" + filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

COMPARE(everything)
COMPARE(test)
COMPARE(hello_world)
COMPARE(transpose)
COMPARE(lots_of_real_code)
COMPARE(comments)
COMPARE(lorem_ipsum)
COMPARE(extra)
COMPARE(backslashes)
