#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include <string>
#include <vector>

namespace ccc {

class ASTNode {
protected:
  ASTNode(int id, std::string name, Token *token = nullptr,
      std::vector<ASTNode *> children_ = std::vector<ASTNode *>())
      : id(id), name(std::move(name)), token(token),
        children(std::move(children_)) {}
  Token *token;
  int id;
  std::string name;
  std::vector<ASTNode *> children;

public:
  int getId() const { return id; }
  virtual std::string toGraphWalker() = 0;
  std::string toGraph() {
    return "graph ast {\nsplines=line;\nstyle=dotted;\nsubgraph cluster{\n" +
      this->toGraphWalker() + "}\n}\n";
  }
  virtual ~ASTNode() {
    for (auto &child : this->children) {
      delete child;
  }
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
