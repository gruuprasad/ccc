#include "../catch.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

// Test simple expressions
TEST_CASE("Fast Parser:primary expression test_1") {
  std::string language{" int main() {"
                       " x = 0;"
                       " return 0;"
                       "}"};

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE(fp.fail() == false);
}

TEST_CASE("Fast Parser:primary expression test_2") {
  {
    std::string language{" int main() {"
      " x = a + b + c;"
        " return 0;"
        "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
  {
    std::string language{" int main() {"
      " x = (a + b - c) * e;"
        " return 0;"
        "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
  {
    std::string language{" int main() {"
      " x = (a + b - c) * e;"
        " y + z - x * p && -q;"
        " sizeof (int);"
        " sizeof (a + b);"
        " sizeof a + b;"
        " a[10];"
        " a->b;"
        " a.b;"
        " x = y + (z.c) + (p->q) - a[10];"
        " x = (y + (z.c) + (p->q)) - a[10];"
        " ((((a.b))));"
        " funcA(1, 2, 3);"
        " return 0;"
        "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
  {
    std::string language{" int main() {"
      " x = \"string\";"
      " y = \'K\';"
        " return 0;"
        "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
  {
    std::string language{" int main() {"
      "if (x) { if (y) return 1; } else {return 2;}"
      "continue;"
      "break;"
      " return 0;"
      "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
}
