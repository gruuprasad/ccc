#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include <string>
#include <vector>

namespace ccc {

class ASTNode {
protected:
  ASTNode(int id, std::string name, unsigned long size = 0,
          Token *token = nullptr,
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
  std::string toGraph();
  virtual ~ASTNode();
};

} // namespace ccc

#endif // C4_ASTNODE_HPP
