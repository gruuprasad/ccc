#include "catch.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "../src/parser/statement.hpp"
#include "../src/parser/declaration.hpp"

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
//TEST_CASE("Our tokenizer should fail on invalid.") {
//  char *args[] = {
//      (char*)"c4",
//      (char*)"--tokenize",
//      (char*)"../examples/error.c",
//      nullptr
//  };
//  REQUIRE(1 == EntryPointHandler().handle(3, args));
//}

TEST_CASE("statement run") {
  int count = 0;
  std::vector<Statement *> scope;

  scope.emplace_back(new IfStatement(count++,
      new ConstantExpression(count++, new IntegerConstant(count++, 8)),
      new ExpressionStatement(count++,
          new AdditiveExpression(count++,
              new ConstantExpression(count++, new CharacterConstant(count++, 'a')),
              new Identifier(count++)))));
  scope.emplace_back(new WhileStatement(count++,
      new ExpressionStatement(count++, new Identifier(count++)),
      new ExpressionStatement(count++, new Identifier(count++))));
  scope.emplace_back(new IfStatement(count++,
      new Identifier(count++),
      new IfStatement(count++,
          new EqualityExpression(count++,
              new ConstantExpression(count++, new IntegerConstant(count++, 42)),
              new ConstantExpression(count++, new IntegerConstant(count++, 3))),
          new ExpressionStatement(count++,
              new ConditionalExpression(count++,
                  new RelationalExpression(count++,
                      new ConstantExpression(count++, new IntegerConstant(count++, 1)),
                      new ConstantExpression(count++, new IntegerConstant(count++, 2))),
                  new MultiplicativeExpression(count++,
                      new ConstantExpression(count++, new IntegerConstant(count++, -1)),
                      new ConstantExpression(count++, new IntegerConstant(count++, 256))),
                  new CharacterConstant(count++, 'Q'))),
          new ExpressionStatement(count++, new AssignmentExpression(count++, new Declaration(count++)))),
      new ExpressionStatement(count++, new Identifier(count++))));
  scope.emplace_back(new ReturnStatement(count++, new Identifier(count++)));

  ASTNode *root = new CompoundStatement(count, scope);
  std::ofstream file;
  file.open("ast.gv");
  file << root->toGraph();
  file.close();

  std::cout << root->toGraph();

  delete root;
}

