#ifndef C4_EXPRESSIONS_HPP
#define C4_EXPRESSIONS_HPP

#include <vector>
#include <iosfwd>
#include <iostream>
#include "ASTNode.hpp"


class Expression : public  ASTNode {
private:
  std::string toGraphRec() override;
public:
  Expression(int id, const std::string &name);
};

class Identifier : public Expression {
public:
  explicit Identifier(int id);
};

class AdditiveExpression : public Expression {
public:
  AdditiveExpression(int id, ASTNode *left, ASTNode *right);
};

#endif // C4_EXPRESSIONS_HPP
