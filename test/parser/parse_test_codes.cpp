#include "../catch.hpp"
#include "parser/fast_parser.hpp"
#include <fstream>
#include <iostream>

TEST_CASE("Read simple unit from .c4") {
  std::ifstream in("../../test/parser/test_codes.c4");
  REQUIRE(in.is_open());
  ccc::Reader reader(in);
  auto unit = reader.readUnit();
  //  std::cout << unit << std::endl;
  REQUIRE(!unit.empty());
}

TEST_CASE("Parse multiple units from .c4") {
  std::ifstream in("../../test/parser/test_codes.c4");
  REQUIRE(in.is_open());
  ccc::Reader reader(in);
  while (true) {
    std::string unit = reader.readUnit();
    if (!unit.empty())
      break;
    auto fp = ccc::FastParser(unit);
    auto root = fp.parse();
    REQUIRE_SUCCESS(fp);
  }
}
