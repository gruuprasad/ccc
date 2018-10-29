#include <iostream>
#include "ASTNode.hpp"

ASTNode::ASTNode(int id, std::string name, Token *token) {
  this->id = id;
  this->name = std::move(name);
  this->token = token;
}

ASTNode::ASTNode(int id, std::string name) {
  this->id = id;
  this->name = std::move(name);
  this->token = nullptr;
}

ASTNode::~ASTNode() {
  for (ASTNode *c : this->children) {
    delete c;
  }
}

std::string ASTNode::toGraph() {
  return "graph ast {\nratio=\"compress\"\nforcelabels=true;\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n"
      + this->toGraphRec() + "}\n}\n";
}

std::string ASTNode::toGraphRec() {
  throw std::bad_function_call();
}

int ASTNode::getId() const {
  return id;
}

ASTNode *ASTNode::getChild(int pos) {
  return this->children[pos];
}

