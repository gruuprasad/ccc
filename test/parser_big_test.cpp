#include "catch.hpp"
#include "../src/parser/fast_parser.hpp"
#include "../src/lexer/fast_lexer.hpp"
#include "../src/entry/entry_point_handler.hpp"

using namespace ccc;

#define PARSE_FILE(filename) \
  TEST_CASE("Parse "#filename) {\
    std::string filedir = "../examples/"; \
    std::string file = filedir + filename; \
    file.push_back('\0'); \
    char * argv[] = {"c4", "--parse", &file[0] };\
    auto rc = EntryPointHandler().handle(3, argv); \
    REQUIRE(rc == 0); \
  } \

PARSE_FILE("declarations.c")

