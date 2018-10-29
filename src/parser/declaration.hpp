#ifndef C4_DECLARATION_HPP
#define C4_DECLARATION_HPP

#include "ast_node.hpp"

class Declaration : public ASTNode {
private:
  std::string toGraphWalker() override;
public:
  explicit Declaration(int id);
};

#endif //C4_DECLARATION_HPP
