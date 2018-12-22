#ifndef C4_GHOST_HPP
#define C4_GHOST_HPP

#include "../lexer/token.hpp"
#include "ast_node.hpp"
#include "expression.hpp"

#include <sstream>
#include <vector>

namespace ccc {

class Ghost : public ASTNode {
private:
public:
  Ghost(int id, const Token *token, ASTNode *child)
      : ASTNode(id, "ghost", 1, token, {child}) {}

  Ghost(int id, const Token *token, std::vector<ASTNode *> children = {})
      : ASTNode(id, "ghost", children.size(), token, children) {}

private:
  std::string toGraphWalker() override {
    if (children.empty() && this->token) {
      std::stringstream ss;
      ss << this->id << "[label=<<font point-size='10'>" << *this->token
         << "</font>";
      ss << "> shape=none style=filled fillcolor=lightgrey];\n";
      return ss.str();
    } else {
      std::stringstream ss;
      for (ASTNode *child : children) {
        ss << child->toGraphWalker();
      }
      return ss.str();
    }
  }
};
} // namespace ccc
#endif // C4_GHOST_HPP
