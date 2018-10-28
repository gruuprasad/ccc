#include "expressions.hpp"

Expression::Expression(int id) : ExpressionStatement(id) {

}

PrimaryExpression::PrimaryExpression(int id) : Expression(id) {
  PrimaryExpression::name = "identifier";
}
