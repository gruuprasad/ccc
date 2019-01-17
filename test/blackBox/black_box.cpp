#include "../catch.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

TEST_CASE("parser/bad_number") {
  std::string ctx = "int (main())\n"
                    "{\n"
                    "\tint a;\n"
                    "\t(a = 9999999999);\n"
                    "}\n";
  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_FAIL(fp);
}

TEST_CASE("lonely type") {
  std::string ctx = "int;\n"
                    "void;\n"
                    "char;\n"
                    "void (a(int, int, int));\n";
  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  //  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), ctx));
}

TEST_CASE("parser/declarators") {
  std::string ctx = "int a;\n";

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), ctx));
}

TEST_CASE("parser/unary_postfix") {
  std::string ctx = "int a;\n";

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), ctx));
}

TEST_CASE("parser/hello") {
  std::string ctx = "int (main())\n"
                    "{\n"
                    "\tchar (*s);\n"
                    "\t(s = \"hello\");\n"
                    "}\n";
  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), ctx));
}

TEST_CASE("parser/digraph") {
  std::string ctx = "int (main<::>)\n"
                    "<%"
                    "\t;\n"
                    "%>\n";
  std::string xtc = "int (main())\n"
                    "{\n"
                    "\t;\n"
                    "}\n";

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_SUCCESS(fp);
  REQUIRE_EMPTY(Utils::compare(root->prettyPrint(0), xtc));
}

TEST_CASE("parser/empty") {
  std::string ctx = "int (main())\n"
                    "{\n"
                    "\tif (1)\n"
                    "}\n";

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  REQUIRE_FAIL(fp);
}
