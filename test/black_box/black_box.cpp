#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "parser/fast_parser.hpp"

#define ROOT_DIR std::string("../../black_box_files/")

using namespace ccc;

TEST_CASE("lexer_failure_files") {
  std::string dir = ROOT_DIR + "lexer_failure_files/";
  for (const auto &file : Utils::dir(&dir[0])) {
    SECTION(file) {
      std::string flag = "--tokenize";
      std::string input = dir + file;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      if (EXIT_SUCCESS == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected pass\033[0m");

      delete[] ppArgs;
    }
  }
}

TEST_CASE("parser_success_files") {
  std::string dir = ROOT_DIR + "parser_success_files/";
  for (const auto &file : Utils::dir(&dir[0])) {
    SECTION(file) {
      std::string flag = "--parse";
      std::string input = dir + file;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      if (EXIT_FAILURE == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected fail\033[0m");

      delete[] ppArgs;
    }
  }
}

TEST_CASE("parser_failure_files") {
  std::string dir = ROOT_DIR + "parser_failure_files/";
  auto files = Utils::dir(&dir[0]);
  for (const auto &file : files) {
    SECTION(file) {
      std::string flag = "--parse";
      std::string input = dir + file;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      if (EXIT_SUCCESS == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected pass\033[0m");

      delete[] ppArgs;
    }
  }
}

TEST_CASE("pretty_printer_files") {
  std::string dir = ROOT_DIR + "pretty_printer_files/";
  for (const auto &file : Utils::dir(&dir[0])) {
    SECTION(file) {
      std::string flag = "--print-ast";
      std::string input = dir + file;

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

      if (EXIT_FAILURE == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected fail\033[0m");

      delete[] ppArgs;

      std::cout.rdbuf(sb);
      std::string content = ss.str();

      REQUIRE_EMPTY(Utils::compare(content, expected));
    }
  }
}
