#include "../catch.hpp"
#include "parser/fast_parser.hpp"

#define REQUIRE_SUCCESS(fp)                                                    \
  if (fp.fail())                                                               \
    FAIL("\033[1;31m" + fp.getError() + "\033[0m");                            \
  SUCCEED();

using namespace ccc;

// Test simple expressions
TEST_CASE("Fast Parser:primary expression test_1") {
  std::string language{" int main() {"
                       " printf(\"hello world!\");"
                       " return 0;"
                       "}"};

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
}

TEST_CASE("Fast Parser:primary expression test_2") {
  {
    std::string language{" int main() {"
                         " x = a + b + c;"
                         " return 0;"
                         "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
  }
  {
    std::string language{" int main() {"
                         " x = (a + b - c) * e;"
                         " return 0;"
                         "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
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
    REQUIRE_SUCCESS(fp);
  }
  {
    std::string language{" int main() {"
                         " x = \"string\";"
                         " y = \'K\';"
                         " return 0;"
                         "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
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
    REQUIRE_SUCCESS(fp);
  }
  {
    std::string language{" int main() {"
                         "a = ((X) + (y) + ((z))) + ((y) + (k));"
                         " return 0;"
                         "}"};

    auto fp = FastParser(language);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
  }
}
