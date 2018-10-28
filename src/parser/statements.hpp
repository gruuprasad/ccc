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
  void toGraph();
  void addChild(Statement *child);
};

class BlockStatement : public Statement {
public:
  explicit BlockStatement(int id);
};

class EmptyStatement : public Statement {
public:
  explicit EmptyStatement(int id);
};

class DeclarationStatement : public Statement {
public:
  explicit DeclarationStatement(int id);
};

class IfStatement : public Statement {
public:
  explicit IfStatement(int id);
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(int id);
};

class ExpressionStatement : public Statement {
public:
  explicit ExpressionStatement(int id);
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(int id);
};

class WhileStatement : public Statement {
public:
  explicit WhileStatement(int id);
};

class ReturnStatement : public Statement {
public:
  explicit ReturnStatement(int id);
};

class ErrorStatement : public Statement {
public:
  explicit ErrorStatement(int id);
};

#endif // C4_STATEMENT_HPP
