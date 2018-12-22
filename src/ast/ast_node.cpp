#include "ast_node.hpp"

#include <utility>
#include <vector>

namespace ccc {

ASTNode::~ASTNode() {
  for (auto &child : this->children) {
    delete child;
  }
}

std::string ASTNode::toGraph() {
  return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
         this->toGraphWalker() + "}\n}\n";
}

} // namespace ccc
