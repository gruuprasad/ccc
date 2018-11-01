

#include <sstream>
#include "expression.hpp"

Expression::Expression(int id, const std::string &name, const unsigned long size) : ASTNode(id, name, size) {
}

std::string Expression::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name << "<br/><font point-size='10'>" << this->name
     << "</font>> shape=oval style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    ss << child->toGraphWalker();
    ss << this->id << " -- " << child->getId()
       << "[taillabel=\"?\" labeldistance=0 labelangle=0 labelfontcolor=red];\n";
  }
  return ss.str();
}

Identifier::Identifier(int id) : Expression(id, "primary-expression", 0) {
}

AdditiveExpression::AdditiveExpression(int id, Expression *expression_1, Expression *expression_2) : Expression(id,
                                                                                                                "additive-expression",
                                                                                                                2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

EqualityExpression::EqualityExpression(int id, Expression *expression_1, Expression *expression_2) : Expression(id,
                                                                                                                "equality-expression",
                                                                                                                2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

ConditionalExpression::ConditionalExpression(int id,
                                             Expression *expression_1,
                                             Expression *expression_2,
                                             Expression *expression_3) : Expression(id, "conditional-expression", 3) {
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

LogicalAndExpression::LogicalAndExpression(int id, ASTNode *expression_1, ASTNode *expression_2) : Expression(id,
                                                                                                              "logical-And-expression",
                                                                                                              2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

LogicalOrExpression::LogicalOrExpression(int id, Expression *expression_1, Expression *expression_2) : Expression(id,
                                                                                                                  "logical-And-expression",
                                                                                                                  2) {
  this->children.push_back(expression_1);
  this->children.push_back(expression_2);
}

AssignmentExpression::AssignmentExpression(int id, Expression *assign, Expression *expression) : Expression(id,
                                                                                                            "assignment-expression",
                                                                                                            2) {
  this->children.push_back(assign);
  this->children.push_back(expression);
}

FunctionCall::FunctionCall(int id, std::vector<ASTNode *> arguments) : Expression(id, "postfix-expression", 1) {
  this->children = std::move(arguments);
}

SizeOfExpression::SizeOfExpression(int id, ASTNode *type) : Expression(id, "unary-expression", 1) {
  this->children.push_back(type);
}

ConstantExpression::ConstantExpression(int id, Constant *constant) : Expression(id, "primary-expression", 1) {
  this->children.push_back(constant);
}
