#include "../catch.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

TEST_CASE("Valid Declaration Tests") {

  SECTION("simple variable declaration") {
    std::string language { "int a; void b; struct C list;" };
    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
  
  SECTION("struct declaration test") {
    SECTION("struct without alias") {
      std::string language { "struct C { void a; int b; char c; };" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    SECTION("struct with alias") {
      std::string language { "struct C { void a; int b; char c; } list_t;" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    SECTION("nested struct member") {
      std::string language { "struct C { void a; int b; char c; struct D { int a; int b; }; } list_t;" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
  }

  SECTION("Function type declaration test") {
    SECTION("prototype without parameters") {
      std::string language { "void funcA();" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    SECTION("prototype with parameters") {
      std::string language { "void funcA(int a);" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    SECTION("prototype with multiple parameters") {
      std::string language { "void funcA(int a, char c);" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    SECTION("prototype with multiple parameters (struct)") {
      std::string language { "void funcA(int a, char c, struct A d);" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
  }

  SECTION("simple pointer type declaration test") {
    SECTION("data type pointers") {
      std::string language { "int *a; char **c;" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    /* TODO Detected bug in parseDeclarator, need to fix.
    SECTION("function type pointers") {
      std::string language { "int (*a) (int a);" };
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE(fp.fail() == false);
    }
    */
  }
}
