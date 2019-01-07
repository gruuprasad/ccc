#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include <iostream>

namespace ccc {

TEST_CASE("scope test") {
  auto root = new TranslationUnit({
      new DeclarationStatement(
          Token(),
          new TypeExpression(TypeSpec::INT, new IdentifierExpression(Token(
                                                TokenType::IDENTIFIER, "a")))),
      new CompoundStatement(
          Token(),
          {
              new DeclarationStatement(
                  Token(),
                  new TypeExpression(TypeSpec::INT,
                                     new IdentifierExpression(
                                         Token(TokenType::IDENTIFIER, "b")))),
          }),
      new DeclarationStatement(
          Token(),
          new TypeExpression(TypeSpec::INT, new IdentifierExpression(Token(
                                                TokenType::IDENTIFIER, "c")))),
  });
  root->checkType();
} // namespace ccc

} // namespace ccc
