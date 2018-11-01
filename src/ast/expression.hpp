#ifndef C4_EXPRESSIONS_HPP
#define C4_EXPRESSIONS_HPP

#include <vector>
#include <iosfwd>
#include <iostream>
#include "ast_node.hpp"
#include "constant.hpp"
#include "declaration.hpp"

class Expression : public ASTNode {
private:
  std::string toGraphWalker() override;
public:
  Expression(int, const std::string &, unsigned long);
};

class Identifier : public Expression {
public:
  explicit Identifier(int);
};

class StringLiteral : public Expression {
};

class PrimaryExpression : public Expression {
};

class AssignmentExpression : public Expression {
public:
  AssignmentExpression(int, Expression *, Expression *);
};

class FunctionCall : public Expression {
public:
  FunctionCall(int, std::vector<ASTNode *>);
};

class SizeOfExpression : public Expression {
public:
  SizeOfExpression(int, ASTNode *);
};

class MultiplicativeExpression : public Expression {
public:
  MultiplicativeExpression(int, Expression *, Expression *);
};

class AdditiveExpression : public Expression {
public:
  AdditiveExpression(int, Expression *, Expression *);
};

class RelationalExpression : public Expression {
public:
  RelationalExpression(int, Expression *, Expression *);
};

class EqualityExpression : public Expression {
public:
  EqualityExpression(int, Expression *, Expression *);
};

class LogicalAndExpression : public Expression {
public:
  LogicalAndExpression(int, ASTNode *, ASTNode *);
};

class LogicalOrExpression : public Expression {
public:
  LogicalOrExpression(int, Expression *, Expression *);
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(int, Expression *, Expression *, Expression *);
};

class ConstantExpression : public Expression {
public:
  ConstantExpression(int, Constant*);
};

#endif // C4_EXPRESSIONS_HPP
