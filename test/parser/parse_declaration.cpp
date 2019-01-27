#include "../catch.hpp"
#include "ast/visitor/pretty_printer.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

TEST_CASE("Multiple struct Declarations") {
  std::string language{"struct vehicle {"
                       "  int n_wheels;"
                       "  void n_seats;"
                       "  int power;"
                       "  char model;"
                       " } vehicle_t;"
                       ""
                       " struct my_car {"
                       " void a;"
                       " void b;"
                       " void c;"
                       " void d;"
                       " void e;"
                       " int total_voids;"
                       " };"
                       "// Declare my_car variable"
                       " struct my_car car_g;"
                       " struct vehicle car_f;"
                       " vehicle_t car_h;"
                       " int total_cars;"
                       " // Forward declare new type"
                       " struct new_model;"};

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
}

TEST_CASE("Function definition") {
  SECTION("simple variable declaration") {
    std::string language{"void funcA() { }"};
    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
  }
}

TEST_CASE("Valid Declaration Tests") {

  SECTION("simple variable declaration") {
    std::string language{"int a; void b; struct C list;"};
    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
  }

  SECTION("struct declaration test") {
    SECTION("struct without alias") {
      std::string language{"struct C { void a; int b; char c; };"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("struct with alias") {
      std::string language{"struct C { void a; int b; char c; } list_t;"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("nested struct member") {
      std::string language{"struct C { void a; int b; char c; struct D { int "
                           "a; int b; }; } list_t;"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
  }

  SECTION("Function type declaration test") {
    SECTION("prototype without parameters") {
      std::string language{"void funcA();"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("prototype with parameters") {
      std::string language{"void funcA(int a);"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("prototype with multiple parameters") {
      std::string language{"void funcA(int a, char c);"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("prototype with multiple parameters (struct)") {
      std::string language{"void funcA(int a, char c, struct A d);"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
  }

  SECTION("simple pointer type declaration test") {
    SECTION("data type pointers") {
      std::string language{"int *a; char **c;"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("function type pointers") {
      std::string language{"int (*a) (int a);"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
    SECTION("parenthesied pointer") {
      std::string language{"int * (a); char (*c);"};
      auto fp = FastParser(language);
      auto root = fp.parse();
      REQUIRE_SUCCESS(fp);
    }
  }
}

TEST_CASE("Function with pointer return type") {
  std::string language{"int *funcA() { ;}"};
  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
}

TEST_CASE("declarations issue") {
  std::string language{"int main() { int a; int b;}"};
  auto fp = FastParser(language);
  auto root = fp.parse();
  //  PrettyPrinterVisitor pp;
  //  std::cout << root->accept(&pp);
  REQUIRE_SUCCESS(fp);
}
