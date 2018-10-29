#ifndef C4_EXPRESSIONS_HPP
#define C4_EXPRESSIONS_HPP

#include <vector>
#include <iosfwd>
#include <iostream>
#include "ast_node.hpp"

class Expression : public ASTNode {
private:
  std::string toGraphWalker() override;
public:
  Expression(int id, const std::string &name, unsigned long size);
};

class Identifier : public Expression {
public:
  explicit Identifier(int id);
};

class StringLiteral : public Expression {
};

class GenericSelection : public Expression {
};

class PostfixExpression : public Expression {
};

class ArgumentExpression : public Expression {
};

class UnaryExpression : public Expression {
};

class SizeOfExpression : public Expression {
};

class AllignOfExpression : public Expression {
};

class CastExpression : public Expression {
};

class MultiplicativeExpression : public Expression {
public:
  MultiplicativeExpression(int id, ASTNode *expression_1, ASTNode *expression_2);
};

class AdditiveExpression : public Expression {
public:
//  AdditiveExpression(int id, ASTNode *expression_1);
  AdditiveExpression(int id, ASTNode *expression_1, ASTNode *expression_2);
};

class ShiftExpression : public Expression {
};

class RelationalExpression : public Expression {
public:
  RelationalExpression(int id, ASTNode *expression_1, ASTNode *expression_2);
};

class EqualityExpression : public Expression {
public:
  EqualityExpression(int id, ASTNode *expression_1, ASTNode *expression_2);
};

class AndExpression : public Expression {
public:
  AndExpression(int id, ASTNode *expression_1, ASTNode *expression_2);
};

class OrExpression : public Expression {
public:
  OrExpression(int id, ASTNode *expression_1, ASTNode *expression_2);
};

class LogicalAndExpression : public Expression {
};

class LogicalOrExpression : public Expression {
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(int id, ASTNode *expression_1, ASTNode *expression_2, ASTNode *expression_3);
};

class AssignmentExpression : public Expression {
};

class ConstantExpression : public Expression {
};

#endif // C4_EXPRESSIONS_HPP
