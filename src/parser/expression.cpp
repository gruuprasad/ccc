#include <utility>

#include <sstream>
#include "expression.hpp"

Expression::Expression(int id, const std::string &name, const unsigned long size) : ASTNode(id, name, size) {
}

std::string Expression::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=ellipse style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    ss << child->toGraphWalker();
    ss << this->id << " -- " << child->getId() << ";\n";
  }
  return ss.str();
}

Identifier::Identifier(int id) : Expression(id, "identifier", 0) {
}

AdditiveExpression::AdditiveExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id,
    "additive-expression",
    2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

EqualityExpression::EqualityExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id,
    "equality-expression",
    2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

AndExpression::AndExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id,
    "AND-expression",
    2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

OrExpression::OrExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id, "AND-expression", 2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

ConditionalExpression::ConditionalExpression(int id,
    ASTNode *expression_1,
    ASTNode *expression_2,
    ASTNode *expression_3) : Expression(id, "conditional-expression", 3) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
  this->children.push_back(expression_3);
}

RelationalExpression::RelationalExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id,
    "conditional-expression",
    2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

MultiplicativeExpression::MultiplicativeExpression(int id, ASTNode *expression_1, ASTNode *expression_2)
    : Expression(id, "multiplicative-expression", 2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}
