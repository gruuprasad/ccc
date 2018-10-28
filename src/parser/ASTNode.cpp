#include <iostream>
#include <sstream>
#include "ASTNode.hpp"

ASTNode::ASTNode(int id, std::string name) {
  this->id = id;
  this->name = std::move(name);
}

void ASTNode::print() {
  std::cout << ASTNode::name << "[";
  for (ASTNode *c : this->children) {
    c->print();
  }
  std::cout << "]";
}

std::string ASTNode::toGraph() {
  return
      "graph ast {\nratio=\"compress\"\nforcelabels=true;\nsplines=line;\nstyle=dotted;\nsubgraph cluster {\nlabel=\"root\"\n"
          + this->toGraphRec()
          + "}\n}\n";
}

std::string ASTNode::toGraphRec() {
  std::stringstream ss;
  ss << "subgraph cluster" << this->id << " {\nlabel = \"\"\n";
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *c : this->children) {
    ss << this->id << " -- " << c->id << ";\n";
    ss << c->toGraphRec();
  }
  return ss.str() + "}\n";
}

void ASTNode::addChild(ASTNode *child) {
  this->children.emplace_back(child);
}