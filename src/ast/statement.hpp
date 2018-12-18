#ifndef C4_STATEMENT_HPP
#define C4_STATEMENT_HPP

#include <iosfwd>
#include <iostream>
#include <vector>

#include "../lexer/token.hpp"
#include "ast_node.hpp"
#include "expression.hpp"

class Statement : public ASTNode {
private:
public:
  Statement(int, const std::string &, unsigned long);
  Statement(int, const std::string &);

private:
  std::string toGraphWalker() override;
};

class LabelStatement : public Statement {
public:
  LabelStatement(int, Expression *, Statement *);
};

class CompoundStatement : public Statement {
public:
  CompoundStatement(int, std::vector<ASTNode *> items);
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(int, Expression *);
  explicit ExpressionStatement(int);
};

class IfStatement : public Statement {
public:
  IfStatement(int, Expression *, Statement *);
};

class IfElseStatement : public Statement {
public:
  IfElseStatement(int, Expression *, Statement *, Statement *);
};

class WhileStatement : public Statement {
public:
  WhileStatement(int, Expression *, Statement *);
};

class GotoStatement : public Statement {
public:
  GotoStatement(int, Expression *);
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(int);
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(int);
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(int, Expression *);
};

#endif // C4_STATEMENT_HPP
