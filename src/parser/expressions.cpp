#include <utility>

#include <sstream>
#include "expressions.hpp"

Expression::Expression(int id, const std::string &name) : ASTNode(id, name) {
}

std::string Expression::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=ellipse style=filled fillcolor=mediumslateblue];\n";
  for (ASTNode *c : this->children) {
    ss << "subgraph cluster_" << c->getId() << "{\n" << c->toGraphRec() << "}\n";
    ss << this->getId() << " -- " << c->getId() << ";\n";
  }
  return ss.str();
}

Identifier::Identifier(int id) : Expression(id, "identifier") {
}

AdditiveExpression::AdditiveExpression(int id, ASTNode *left, ASTNode *right) : Expression(id, "additive-expression") {
  this->children.emplace_back(left);
  this->children.emplace_back(right);
}
