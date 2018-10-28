#ifndef C4_STATEMENT_HPP
#define C4_STATEMENT_HPP

#include <vector>
#include <iosfwd>
#include <iostream>

class Statement {
protected:
  int id = -1;
  std::vector<Statement *> children;
  std::string name;
  std::string toGraphRec();

public:
  Statement(int id, std::string name);
  void print();
  std::string toGraph();
  void addChild(Statement *child);
};

class LabeledStatement : public Statement {
public:
  explicit LabeledStatement(int id);
};

class CompoundStatement : public Statement {
public:
  explicit CompoundStatement(int id);
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(int id);
};

class SelectionStatement : public Statement {
public:
  explicit SelectionStatement(int id);
};

class IterationStatement : public Statement {
public:
  explicit IterationStatement(int id);
};

class JumpStatement : public Statement {
public:
  explicit JumpStatement(int id);
};

#endif // C4_STATEMENT_HPP
