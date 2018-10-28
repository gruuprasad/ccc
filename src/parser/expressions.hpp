#ifndef C4_EXPRESSIONS_HPP
#define C4_EXPRESSIONS_HPP

#include <vector>
#include <iosfwd>
#include <iostream>

#include "statements.hpp"

class Expression : public ExpressionStatement {
public:
  explicit Expression(int id);
};

class PrimaryExpression : public Expression {
public:
  explicit PrimaryExpression(int id);
};

#endif // C4_EXPRESSIONS_HPP
