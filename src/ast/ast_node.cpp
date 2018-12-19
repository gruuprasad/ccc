#include "ast_node.hpp"

ASTNode::ASTNode(int id, std::string name, ccc::Token *token,
                 const unsigned long size) {
  this->id = id;
  this->name = std::move(name);
  this->token = token;
  this->children.reserve(size);
}

ASTNode::ASTNode(int id, std::string name, const unsigned long size) {
  this->id = id;
  this->name = std::move(name);
  this->token = nullptr;
  this->children.reserve(size);
}

ASTNode::ASTNode(int id, std::string name) {
  this->id = id;
  this->name = std::move(name);
  this->token = nullptr;
}

ASTNode::~ASTNode() {
  for (ASTNode *child : this->children) {
    delete child;
  }
}

std::string ASTNode::toGraph() {
  return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
         this->toGraphWalker() + "}\n}\n";
}

int ASTNode::getId() const { return id; }
