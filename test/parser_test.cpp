#include "catch.hpp"
#include "../src/parser/fast_parser.hpp"
#include "../src/lexer/fast_lexer.hpp"

using namespace ccc;

template<typename T>
void print(const T& t) {
  for (auto & elem : t)
    std::cout << elem << "  ";
  std::cout << "\n";
}
#define DECLARATION_TESTS(id, language, valid) \
TEST_CASE("Fast Parser simple declaration test "#id) { \
    auto token_list = FastLexer(language).lex(); \
    print(token_list); \
    auto fp = FastParser(token_list); \
    fp.parse(); \
    REQUIRE((!fp.fail()) == valid); \
} \

DECLARATION_TESTS(1, "int a;", true)
DECLARATION_TESTS(2, "struct a;", true)
