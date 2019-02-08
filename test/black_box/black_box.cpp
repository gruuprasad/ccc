#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "parser/fast_parser.hpp"

#define ROOT_DIR std::string("../../black_box_files/")

#define GCC_SUCCESS                                                            \
  std::string gcc =                                                            \
      "../../llvm/install/bin/clang -std=c11 -w -c " + input + " 2>&1";        \
  system(&gcc[0]);

#define GCC_FAILURE                                                            \
  std::string gcc = "gcc -pedantic-errors -std=c11 -w -c " + input + " 2>&1";  \
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
    std::cerr << "black box c4 " << std::endl                                  \
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

TEST_CASE("help") {
  std::string flag = "--help";
  std::cout << "./c4 " << flag << std::endl;
  char **ppArgs = new char *[2];
  ppArgs[1] = &flag[0];
  EntryPointHandler().handle(2, ppArgs);
  delete[] ppArgs;
}

TEST_CASE("lexer_failure_files") {
  std::string dir = ROOT_DIR + "lexer_failure_files/";
  for (const auto &file : Utils::dir(&dir[0])) {
    SECTION(file) {
      std::string flag = "--tokenize";
      std::string input = dir + file;
      std::cout << "./c4 " << flag << " " << input << std::endl;

      GCC_FAILURE;
      PIPE_CERR;
      PIPE_COUT;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      int ret = EntryPointHandler().handle(3, ppArgs);

      PIPE_COUT_RESET;
      PIPE_CERR_RESET;
      GCC_DIFF;

      if (ret == EXIT_SUCCESS)
        FAIL(input + ":0:0: Unexpected pass");
      REQUIRE(ret == EXIT_FAILURE);
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
      std::cout << "./c4 " << flag << " " << input << std::endl;

      //      GCC_SUCCESS;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      int ret = EntryPointHandler().handle(3, ppArgs);

      if (ret == EXIT_FAILURE)
        FAIL(input + ":0:0: Unexpected fail");
      REQUIRE(ret == EXIT_SUCCESS);
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
      std::cout << "./c4 " << flag << " " << input << std::endl;

      GCC_FAILURE;
      PIPE_CERR;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      int ret = EntryPointHandler().handle(3, ppArgs);

      PIPE_CERR_RESET;
      GCC_DIFF;

      if (ret == EXIT_SUCCESS)
        FAIL(input + ":0:0: Unexpected pass");
      REQUIRE(ret == EXIT_FAILURE);
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
      std::cout << "./c4 " << flag << " " << input << std::endl;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      GCC_SUCCESS;
      PIPE_COUT;

      int ret = EntryPointHandler().handle(3, ppArgs);

      PIPE_COUT_RESET;

      if (ret == EXIT_FAILURE)
        FAIL(input + ":0:0: Unexpected fail");
      REQUIRE(ret == EXIT_SUCCESS);

      std::ifstream ifs(input);
      std::stringstream buffer;
      buffer << ifs.rdbuf();
      std::string expected = buffer.str();

      REQUIRE_EMPTY(Utils::compare(content, expected));

      delete[] ppArgs;
    }
  }
}

TEST_CASE("compiler_success_files") {
  std::string dir = ROOT_DIR + "compiler_success_files/local/";
  for (const auto &file : Utils::dir(&dir[0])) {
    SECTION(file) {
      std::string flag = "--compile";
      std::string input = dir + file;
      std::cout << "./c4 " << flag << " " << input << std::endl;

      //      GCC_SUCCESS;

      char **ppArgs = new char *[3];
      ppArgs[1] = &flag[0];
      ppArgs[2] = &input[0];

      int ret = EntryPointHandler().handle(3, ppArgs);

      if (ret == EXIT_FAILURE)
        FAIL(input + ":0:0: Unexpected fail");

      std::string cmd = "../../llvm/install/bin/clang -w -o " +
                        file.substr(0, file.rfind(".c")) + " " +
                        file.substr(0, file.rfind(".c")) + ".ll && ./" +
                        file.substr(0, file.rfind(".c"));
      std::cout << cmd << std::endl;
      int run = system(&cmd[0]);
      run = WEXITSTATUS(run);

      cmd = "../../llvm/install/bin/clang -w -o " +
            file.substr(0, file.rfind(".c")) + " " + input + " && ./" +
            file.substr(0, file.rfind(".c"));

      int ref = system(&cmd[0]);
      ref = WEXITSTATUS(ref);

      if (run != ref)
        std::cerr << input << ":0:0: Unexpected diff" << std::endl;
      REQUIRE(run == ref);
      delete[] ppArgs;
    }
  }
}
