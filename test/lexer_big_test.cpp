#include <iostream>
#include <fstream>
#include <sstream>
#include "catch.hpp"
#include "../src/lexer/fast_lexer.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "../src/lexer/lexer_exception.hpp"
#include <iterator>
#include <zconf.h>

using namespace ccc;

#define COMPARE(name) \
TEST_CASE("Compare "#name".c to "#name".txt") { \
  REQUIRE(lexing_of(#name".c", to_match(#name".txt"))); \
}

std::vector<std::string> split_lines(const std::string &str) {
  std::stringstream ss(str);
  std::string tmp;
  std::vector<std::string> split;
  while (std::getline(ss, tmp, '\n')) {
    while (tmp[tmp.length() - 1] == '\r' || tmp[tmp.length() - 1] == '\n') {
      tmp.pop_back();
    }
    split.push_back(tmp);
  }
  return split;
}

bool lexing_of(const std::string &filename, const std::string &expected) {
  fpos_t pos;
  fgetpos(stdout, &pos);
  int fd = dup(fileno(stdout));

  freopen("stdout.tmp", "w", stdout);
  EntryPointHandler().tokenize(std::ifstream("../examples/" + filename), filename);

  fflush(stdout);
  dup2(fd, fileno(stdout));
  close(fd);
  clearerr(stdout);
  fsetpos(stdout, &pos);

  std::ifstream t("stdout.tmp");

  std::vector<std::string> content_lines;
  std::vector<std::string> expected_lines = split_lines(expected);

  std::string str;
  while (std::getline(t, str)) {
    content_lines.push_back(str);
  }

  t.close();
  remove("stdout.tmp");

  unsigned int counter = 0;
  for (unsigned long i = 0; i < std::max(content_lines.size(), expected_lines.size()); i++) {
    if (content_lines[i] == expected_lines[i]) {
      continue;
    }
    if (i >= expected_lines.size()) {
      std::cerr << filename << ": expected nothing but got \"" << content_lines[i] << "\"." << std::endl;
    } else if (i >= content_lines.size()) {
      std::cerr << filename << ": expected \"" << expected_lines[i] << "\" but got nothing." << std::endl;
    } else if (content_lines[i] != expected_lines[i]) {
      std::cerr << filename << ": expected \"" << expected_lines[i] << "\" but got \"" << content_lines[i] << "\"."
                << std::endl;
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
//COMPARE(lots_of_real_code) // TODO fix filename
COMPARE(comments)
COMPARE(lorem_ipsum)
COMPARE(extra)
COMPARE(backslashes)


