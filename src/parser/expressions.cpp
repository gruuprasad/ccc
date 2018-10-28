#include <sstream>
#include "expressions.hpp"

Identifier::Identifier(int id) : ASTNode(id, "identifier") {
  this->id = id;
  this->name = "identifier";
}
std::string Identifier::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=ellipse style=filled fillcolor=mediumslateblue];\n";
  return ss.str();
}
AdditiveExpression::AdditiveExpression(int id, ASTNode *left, ASTNode *right) : ASTNode(id, "additive-expression") {
  this->left = left;
  this->right = right;
}
std::string AdditiveExpression::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->left->id << "{\nlabel = \"\"\n" << this->left->toGraphRec() << "}\n";
  ss << this->id << " -- " << this->left->id << ";\n";
  ss << "subgraph cluster_" << this->right->id << "{\nlabel = \"\"\n" << this->right->toGraphRec() << "}\n";
  ss << this->id << " -- " << this->right->id << ";\n";
  return ss.str();
}
