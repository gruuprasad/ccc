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
  Expression(int id, std::string name, Token *token = nullptr, 
              std::vector<ASTNode *> children = {})
      : ASTNode(id, name, token, children) {}
};

class Identifier : public Expression {
public:
  explicit Identifier(int id) : Expression(id, "primary-expression") {}
};

class StringLiteral : public Expression {};

class PrimaryExpression : public Expression {};

class AssignmentExpression : public Expression {
public:
  AssignmentExpression(int id, Expression *assign, Expression *expression)
      : Expression(id, "assignment-expression", nullptr, {assign, expression})
      {}
};

class FunctionCall : public Expression {
public:
  FunctionCall(int id, std::vector<ASTNode *> arguments)
      : Expression(id, "postfix-expression", nullptr, arguments) {}
};

class SizeOfExpression : public Expression {
public:
  SizeOfExpression(int id, ASTNode *type)
      : Expression(id, "unary-expression", nullptr,
                   std::vector<ASTNode *>{type}) {}
};

class MultiplicativeExpression : public Expression {
public:
  MultiplicativeExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "multiplicative-expression", nullptr,
                   {expr1, expr2}) {}
};

class AdditiveExpression : public Expression {
public:
  AdditiveExpression(int, Expression *expr1, Expression *expr2)
      : Expression(id, "additive-expression", nullptr, {expr1, expr2}) {}
};

class RelationalExpression : public Expression {
public:
  RelationalExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "relational-expression", nullptr, {expr1, expr2}) {}
};

class EqualityExpression : public Expression {
public:
  EqualityExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "equality-expression", nullptr, {expr1, expr2}) {}
};

class LogicalAndExpression : public Expression {
public:
  LogicalAndExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "logical-And-expression", nullptr, {expr1, expr2}) {}
};

class LogicalOrExpression : public Expression {
public:
  LogicalOrExpression(int id, Expression *expr1, Expression *expr2)
      : Expression(id, "logical-Or-expression", nullptr, {expr1, expr2}) {}
};

class ConditionalExpression : public Expression {
public:
  ConditionalExpression(int id, Expression *expr1, Expression *expr2,
                        Expression *expr3)
      : Expression(id, "conditional-expression", nullptr,
                   {expr1, expr2, expr3}) {}
};

class ConstantExpression : public Expression {
public:
  ConstantExpression(int id, Constant *constant)
      : Expression(id, "primary-expression", nullptr,
                   std::vector<ASTNode *>{constant}) {}
};

} // namespace ccc

#endif // C4_EXPRESSIONS_HPP
