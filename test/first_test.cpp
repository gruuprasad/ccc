#include "catch.hpp"
#include "../src/entry/entry_point_handler.hpp"

//TEST_CASE("Our compiler should fail for now.") {
//  REQUIRE(1 == EntryPointHandler().handle(0, nullptr));
//}
//
//TEST_CASE("Our tokenizer should work.") {
//  char *args[] = {
//      (char*)"c4",
//      (char*)"--tokenize",
//      (char*)"../examples/test.c",
//      nullptr
//  };
//  REQUIRE(0 == EntryPointHandler().handle(3, args));
//}
//
/* FIXME Disabling temporarily, passes in debug session, not working well with ctest
TEST_CASE("Our tokenizer should fail on invalid.") {
  char *args[] = {
      (char*)"c4",
      (char*)"--tokenize",
      (char*)"../examples/error.c",
      nullptr
  };
  REQUIRE(1 == EntryPointHandler().handle(3, args));
}

TEST_CASE("Our parser should fail on invalid.") {
  char *args[] = {
      (char*)"c4",
      (char*)"--parse",
      (char*)"../examples/error.c",
      nullptr
  };
  REQUIRE(1 == EntryPointHandler().handle(3, args));
}
*/
