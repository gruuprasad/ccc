#include "../catch.hpp"
#include "entry/entry_point_handler.hpp"
#include "lexer/fast_lexer.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

#define PARSE_FILE(filename)                                                   \
  TEST_CASE("Parse " #filename) {                                              \
    std::string filedir = "../examples/";                                      \
    std::string file = filedir + filename;                                     \
    file.push_back('\0');                                                      \
    std::string flag = "--parse";                                              \
    char **ppArgs = new char *[3];                                             \
    ppArgs[1] = &flag[0];                                                      \
    ppArgs[2] = &file[0];                                                      \
    auto rc = EntryPointHandler().handle(3, ppArgs);                           \
    REQUIRE(rc == EXIT_FAILURE);                                               \
  }

PARSE_FILE("declarations.c")
