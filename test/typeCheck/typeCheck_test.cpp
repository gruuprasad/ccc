#include "../catch.hpp"
#include "ast/scope_handler.hpp"
#include <iostream>

namespace ccc {

TEST_CASE("scope test") {
  ScopeHandler *scope = new ScopeHandler();
  scope->insertDeclaration("a", "test");
  scope->printScope();
  scope->openScope();
  scope->insertDeclaration("a", "test");
  scope->insertDeclaration("b", "test");
  scope->printScope();
  scope->closeScope();
  scope->printScope();
  scope->openScope();
  scope->insertDeclaration("c", "test");
  scope->insertDeclaration("d", "test");
  scope->printScope();
}

} // namespace ccc
