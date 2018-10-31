#include <utility>

#include <sstream>
#include "expression.hpp"

Expression::Expression(int id, const std::string &name, const unsigned long size) : ASTNode(id, name, size) {
}

std::string Expression::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name << "<br/><font point-size='10'>" << this->name << "</font>> shape=oval style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    ss << child->toGraphWalker();
    ss << this->id << " -- " << child->getId() << "[taillabel=\"?\" labeldistance=0 labelangle=0 labelfontcolor=red];\n";
  }
  return ss.str();
}

Identifier::Identifier(int id) : Expression(id, "identifier", 0) {
}

AdditiveExpression::AdditiveExpression(int id, Expression *expression_1, Expression *expression_2) : Expression(id,
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

RelationalExpression::RelationalExpression(int id, Expression *expression_1, Expression *expression_2) : Expression(id,
    "relational-expression",
    2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

MultiplicativeExpression::MultiplicativeExpression(int id, Expression *expression_1, Expression *expression_2)
    : Expression(id, "multiplicative-expression", 2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

LogicalAndExpression::LogicalAndExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id, "logical-And-expression", 2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

LogicalOrExpression::LogicalOrExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id, "logical-And-expression", 2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

ShiftExpression::ShiftExpression(int id, ASTNode *expression_1, ASTNode *expression_2, ASTNode *expression_3) : Expression(id, "logical-And-expression", 2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

ConstantExpression::ConstantExpression(int id, Constant *constant) : Expression(id, "constant-expression", 1) {
  this->children.push_back(constant);
}

AssignmentExpression::AssignmentExpression(int id, ASTNode *assignment) : Expression(id, "assignment-expression", 1) {
  this->children.push_back(assignment);
}
