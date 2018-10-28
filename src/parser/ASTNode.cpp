#include <iostream>
#include <sstream>
#include <functional>
#include "ASTNode.hpp"

ASTNode::ASTNode(int id, std::string name) {
  this->id = id;
  this->name = std::move(name);
}

std::string ASTNode::toGraph() {
  return
      "graph ast {\nratio=\"compress\"\nforcelabels=true;\nsplines=line;\nstyle=dotted;\nsubgraph cluster {\nlabel=\"root\"\n"
          + this->toGraphRec()
          + "}\n}\n";
}
std::string ASTNode::toGraphRec() {
  throw std::bad_function_call();
}
int ASTNode::getId() const {
  return id;
}
