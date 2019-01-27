#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "parser/fast_parser.hpp"

#define ROOT_DIR std::string("../../black_box_files/")

#define GCC_SUCCESS                                                            \
  std::string gcc = "cc -w -c " + input + " 2>&1";                             \
  std::string error = Utils::exec(&gcc[0]);                                    \
  if (!error.empty()) {                                                        \
    while (error.find("error") > error.find_first_of('\n')) {                  \
      error = error.substr(error.find('\n') + 1, error.size());                \
    }                                                                          \
    error = error.substr(error.find(input), error.find_first_of('\n'));        \
    std::cerr << "black box c4 " << flag << std::endl                          \
              << "gcc: " << error << std::endl                                 \
              << std::endl;                                                    \
    FAIL("\033[1;31mUnexpected gcc fail\033[0m");                              \
  }

#define GCC_FAILURE                                                            \
  std::string gcc = "gcc -w -c " + input + " 2>&1";                            \
  std::string error = Utils::exec(&gcc[0]);                                    \
  if (!error.empty()) {                                                        \
    while (error.find("error") > error.find_first_of('\n')) {                  \
      error = error.substr(error.find('\n') + 1, error.size());                \
    }                                                                          \
    error = error.substr(error.find(input), error.find_first_of('\n'));        \
  }

#define GCC_DIFF                                                               \
  if (error.substr(0, error.find("error")) !=                                  \
      error_content.substr(0, error_content.find("error"))) {                  \
    std::cerr << "black box c4 " << flag << std::endl                          \
              << "gcc: " << error << std::endl                                 \
              << "ccc: " << error_content << "\n"                              \
              << std::endl;                                                    \
  }

#define PIPE_CERR                                                              \
  std::stringstream se;                                                        \
  std::streambuf *sb = std::cerr.rdbuf();                                      \
  std::cerr.rdbuf(se.rdbuf());

#define PIPE_CERR_RESET                                                        \
  std::cerr.rdbuf(sb);                                                         \
  std::string error_content = se.str();                                        \
  error_content = error_content.substr(0, error_content.find('\n'));

#define PIPE_COUT                                                              \
  std::stringstream ss;                                                        \
  std::streambuf *sa = std::cout.rdbuf();                                      \
  std::cout.rdbuf(ss.rdbuf());

#define PIPE_COUT_RESET                                                        \
  std::cout.rdbuf(sa);                                                         \
  std::string content = ss.str();

using namespace ccc;

TEST_CASE("list dir") {
  std::string dir = ROOT_DIR;
  for (const auto &file : Utils::dir(&dir[0])) {
    REQUIRE(!file.empty());
    std::cout << dir << file << "\n";
  }
  std::cout << std::endl;
}

TEST_CASE("lexer_failure_files") {
  std::string dir = ROOT_DIR + "lexer_failure_files/";
  for (const auto &file : Utils::dir(&dir[0])) {
    SECTION(file) {
      std::string flag = "--tokenize";
      std::string input = dir + file;

      GCC_FAILURE;
      PIPE_CERR;
      PIPE_COUT;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];
      if (EXIT_SUCCESS == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected pass\033[0m");

      PIPE_COUT_RESET;
      PIPE_CERR_RESET;
      GCC_DIFF;

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

      GCC_SUCCESS;
      PIPE_COUT;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      if (EXIT_FAILURE == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected fail\033[0m");

      PIPE_COUT_RESET;

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

      GCC_FAILURE;
      PIPE_CERR;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      if (EXIT_SUCCESS == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected pass\033[0m");

      PIPE_CERR_RESET;
      GCC_DIFF;

      delete[] ppArgs;
    }
  }
}

TEST_CASE("semantic_failure_files") {
  std::string dir = ROOT_DIR + "semantic_failure_files/";
  auto files = Utils::dir(&dir[0]);
  for (const auto &file : files) {
    SECTION(file) {
      std::string flag = "--parse";
      std::string input = dir + file;

      GCC_FAILURE;
      PIPE_CERR;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      if (EXIT_SUCCESS == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected pass\033[0m");

      PIPE_CERR_RESET;
      GCC_DIFF;

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

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      GCC_SUCCESS;
      PIPE_COUT;

      if (EXIT_FAILURE == EntryPointHandler().handle(3, ppArgs))
        FAIL("\033[1;31mUnexpected fail\033[0m");

      PIPE_COUT_RESET;

      std::ifstream ifs(input);
      std::stringstream buffer;
      buffer << ifs.rdbuf();
      std::string expected = buffer.str();

      REQUIRE_EMPTY(Utils::compare(content, expected));

      delete[] ppArgs;
    }
  }
}
