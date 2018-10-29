#include <sstream>
#include "expressions.hpp"

Identifier::Identifier(int id) : ASTNode(id, "identifier") {
}

std::string Identifier::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=ellipse style=filled fillcolor=mediumslateblue];\n";
  return ss.str();
}

AdditiveExpression::AdditiveExpression(int id, ASTNode *left, ASTNode *right) : ASTNode(id, "additive-expression") {
  this->left = left;
  this->right = right;
}

std::string AdditiveExpression::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->left->getId() << "{\nlabel = \"\"\n" << this->left->toGraphRec() << "}\n";
  ss << this->getId() << " -- " << this->left->getId() << ";\n";
  ss << "subgraph cluster_" << this->right->getId() << "{\nlabel = \"\"\n" << this->right->toGraphRec() << "}\n";
  ss << this->getId() << " -- " << this->right->getId() << ";\n";
  return ss.str();
}
