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
        " return 0;"
        "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE(fp.fail() == false);
  }
}
