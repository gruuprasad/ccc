/*
#include "../catch.hpp"
#include "ast/ast_node.hpp"
#include <iostream>

namespace ccc {

TEST_CASE("scope test") {
  auto root = new TranslationUnit(
      {new DeclarationStatement(
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
       new CompoundStatement(
           Token(), {
                        new ExpressionStatement(
                            Token(), new BinaryExpression(
                                         Token(TokenType::PLUS),
                                         new IdentifierExpression(
                                             Token(TokenType::IDENTIFIER, "a")),
                                         new IdentifierExpression(Token(
                                             TokenType::IDENTIFIER, "d")))),
                    })});
  std::cout << root->prettyPrint(0) << std::endl;
  root->runAnalysis();
} // namespace ccc

} // namespace ccc
*/
