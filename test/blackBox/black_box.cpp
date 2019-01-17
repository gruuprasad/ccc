#include "../catch.hpp"
#include "parser/fast_parser.hpp"

using namespace ccc;

TEST_CASE("emptyStatement") {
  std::string ctx = "void (main(int))\n"
                    "{\n"
                    "\tvoid  c;\n"
                    "}\n";

  auto fp = FastParser(ctx);
  auto root = fp.parse();
  if (fp.fail())
    FAIL("\033[1;31m" + fp.getError() + "\033[0m");
  std::string error = Utils::compare(root->prettyPrint(0), ctx);
  REQUIRE_EMPTY(error);
}
