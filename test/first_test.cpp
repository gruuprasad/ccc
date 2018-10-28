#include "catch.hpp"
#include "../src/entry/entry_point_handler.hpp"
#include "../src/parser/statements.hpp"
#include "../src/parser/expressions.hpp"

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
  Statement *root = new CompoundStatement(count++);

  Statement *i = new SelectionStatement(count++);
  i->addChild(new ExpressionStatement(count++));
  root->addChild(i);

  Statement *w = new IterationStatement(count++);
  w->addChild(new ExpressionStatement(count++));
  Statement *wi = new SelectionStatement(count++);
  wi->addChild(new JumpStatement(count++));
  wi->addChild(new LabeledStatement(count++));
  w->addChild(wi);
  root->addChild(w);

  Statement *e = new ExpressionStatement(count++);
  e->addChild(new PrimaryExpression(count));
  root->addChild(e);

  std::ofstream file;
  file.open("ex.dot");
  file << root->toGraph();
  file.close();

  std::cout << root->toGraph();
}

