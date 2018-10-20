#include "catch.hpp"
#include "../src/entry/entry_point_handler.hpp"

TEST_CASE("Our compiler should fail for now.") {
  REQUIRE(1 == EntryPointHandler().handle(0, nullptr));
}
