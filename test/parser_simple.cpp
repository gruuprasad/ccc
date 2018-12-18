#include "../src/ast/ast_node.hpp"
#include "../src/ast/constant.hpp"
#include "../src/ast/declaration.hpp"
#include "../src/ast/expression.hpp"
#include "../src/ast/statement.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "catch.hpp"

TEST_CASE("generate ast") {
  int count = 0;
  std::vector<ASTNode *> scope;

  scope.emplace_back(new IfStatement(
      count++, new ConstantExpression(count++, new IntegerConstant(count++, 8)),
      new ExpressionStatement(
          count++, new AdditiveExpression(
                       count++,
                       new ConstantExpression(
                           count++, new CharacterConstant(count++, 'a')),
                       new Identifier(count++)))));
  scope.emplace_back(new WhileStatement(
      count++, new Identifier(count++),
      new ExpressionStatement(count++, new Identifier(count++))));
  scope.emplace_back(new IfElseStatement(
      count++, new Identifier(count++),
      new IfElseStatement(
          count++,
          new EqualityExpression(
              count++,
              new ConstantExpression(count++, new IntegerConstant(count++, 42)),
              new ConstantExpression(count++, new IntegerConstant(count++, 3))),
          new ExpressionStatement(
              count++, new ConditionalExpression(
                           count++,
                           new RelationalExpression(
                               count++,
                               new ConstantExpression(
                                   count++, new IntegerConstant(count++, 1)),
                               new ConstantExpression(
                                   count++, new IntegerConstant(count++, 2))),
                           new MultiplicativeExpression(
                               count++,
                               new ConstantExpression(
                                   count++, new IntegerConstant(count++, -1)),
                               new ConstantExpression(
                                   count++, new IntegerConstant(count++, 256))),
                           new ConstantExpression(
                               count++, new CharacterConstant(count++, 'Q')))),
          new ExpressionStatement(
              count++, new AssignmentExpression(
                           count++, new Identifier(count++),
                           new ConstantExpression(
                               count++, new CharacterConstant(count++, 'u'))))),
      new ExpressionStatement(count++, new Identifier(count++))));
  scope.emplace_back(new ReturnStatement(count++, new Identifier(count++)));

  ASTNode *root = new CompoundStatement(count, scope);
  std::ofstream file;
  file.open("ast.gv");
  file << root->toGraph();
  file.close();

  //  std::cout << root->toGraph();

  delete root;
}