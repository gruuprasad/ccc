#include "ast_node.hpp"

#include <utility>

namespace ccc {

ASTNode::ASTNode(int id, std::string name, const unsigned long size,
                 Token *token)
    : id(id), name(std::move(name)), token(token) {
  children.reserve(size);
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

} // namespace ccc
