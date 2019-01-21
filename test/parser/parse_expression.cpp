#include "../catch.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

// Test simple expressions
TEST_CASE("Fast Parser:primary expression test_1") {
  std::string language{" int main(int argc, char ** argv) {"
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

TEST_CASE("parser/digraph") {
  std::string ctx = "int (main())\n"
                    "<%\n"
                    "a[0];\n"
                    "a<:0:>;\n"
                    "%>\n";
  std::string xtc = "int (main())\n"
                    "{\n"
                    "\t(a[0]);\n"
                    "\t(a[0]);\n"
                    "}\n";

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), xtc));
}

// Test simple expressions
TEST_CASE("Fast Parser:bad number") {
  std::string language{" int main() {"
                       " x = 0123;"
                       " return 0;"
                       "}"};

  auto fp = FastParser(language);
  auto root = fp.parse();
  REQUIRE(fp.fail());
}

// Test parser failed tests
TEST_CASE("parser/unary_postfix") {
  std::string ctx{" int main() {"
                  " &a[1];"
                  " x = *fun();"
                  " x = y = z;"
                  " return 0;"
                  "}"};

  std::string xtc{"int (main())\n"
                  "{\n"
                  "\t(&(a[1]));\n"
                  "\t(x = (*(fun())));\n"
                  "\t(x = (y = z));\n"
                  "\treturn 0;\n"
                  "}\n"};

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), xtc));
  REQUIRE(fp.fail() == false);
}

TEST_CASE("ast/ternary") {
  std::string ctx{" int main() {"
                  "m = x ? y : z;"
                  " return 0;"
                  "}"};

  std::string xtc{"int (main())\n"
                  "{\n"
                  "\t(m = (x ? y : z));\n"
                  "\treturn 0;\n"
                  "}\n"};

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), xtc));
  REQUIRE(fp.fail() == false);
}
