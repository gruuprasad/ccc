#ifndef C4_EXPRESSIONS_HPP
#define C4_EXPRESSIONS_HPP

#include "ast_node.hpp"
#include "constant.hpp"
#include "declaration.hpp"

#include <string>
#include <vector>

namespace ccc {

class Expression : public ASTNode {
private:
  std::string toGraphWalker() override;

public:
  Expression(int id, std::string name, unsigned long size,
             const Token *token = nullptr, std::vector<ASTNode *> children = {})
      : ASTNode(id, name, size, token, children) {}
};

class Identifier : public Expression {
public:
  explicit Identifier(int id) : Expression(id, "primary-expression", 0) {}
};

class StringLiteral : public Expression {};

class PrimaryExpression : public Expression {
public:
  explicit PrimaryExpression(int id)
      : Expression(id, "primary expression", 1, nullptr, {}){};
};

class AssignmentExpression : public Expression {
public:
  AssignmentExpression(int id, Expression *assign, const Token *token,
                       Expression *expression)
      : Expression(id, "assignment-expression", 2, token,
                   {assign, expression}) {}
};

class FunctionCall : public Expression {
public:
  FunctionCall(int id, std::vector<ASTNode *> arguments)
      : Expression(id, "postfix-expression", 1, nullptr, arguments) {}
};

class SizeOfExpression : public Expression {
public:
  SizeOfExpression(int id, ASTNode *type)
      : Expression(id, "unary-expression", 1, nullptr,
                   std::vector<ASTNode *>{type}) {}
};

class MultiplicativeExpression : public Expression {
public:
  MultiplicativeExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "multiplicative-expression", 2, nullptr,
                   {expr1, expr2}) {}
};

class AdditiveExpression : public Expression {
public:
  AdditiveExpression(int, Expression *expr1, Expression *expr2)
      : Expression(id, "additive-expression", 2, nullptr, {expr1, expr2}) {}
};

class RelationalExpression : public Expression {
public:
  RelationalExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "relational-expression", 2, nullptr, {expr1, expr2}) {}
};

class EqualityExpression : public Expression {
public:
  EqualityExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "equality-expression", 2, nullptr, {expr1, expr2}) {}
};

class LogicalAndExpression : public Expression {
public:
  LogicalAndExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "logical-And-expression", 2, nullptr, {expr1, expr2}) {}
};

class LogicalOrExpression : public Expression {
public:
  LogicalOrExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "logical-Or-expression", 2, nullptr, {expr1, expr2}) {}
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(int id, Expression *expr1, Expression *expr2,
                        Expression *expr3)
      : Expression(id, "conditional-expression", 3, nullptr,
                   {expr1, expr2, expr3}) {}
};

class ConstantExpression : public Expression {
public:
  ConstantExpression(int id, Constant *constant)
      : Expression(id, "primary-expression", 1, nullptr,
                   std::vector<ASTNode *>{constant}) {}
};

} // namespace ccc

#endif // C4_EXPRESSIONS_HPP
