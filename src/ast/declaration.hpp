#ifndef C4_DECLARATION_HPP
#define C4_DECLARATION_HPP

#include "ast_node.hpp"
#include "type_specifier.hpp"

#include <sstream>
#include <string>

namespace ccc {

class Declaration : public ASTNode {
private:
  ASTNode *ident;
  TypeSpecifier type;
  std::string toGraphWalker() override {
    std::stringstream ss;
    ss << this->id << "[label=\"" << this->name
      << "\" shape=box style=filled fillcolor=lightsalmon];\n";
    return ss.str();
  }

public:
  Declaration(int id, ASTNode *ident, TypeSpecifier type)
      : ASTNode(id, "declaration"), ident(ident), type(type) {}
};

class InitDeclaration : Declaration {
public:
  InitDeclaration(int id, ASTNode *ident, TypeSpecifier type)
      : Declaration(id, ident, type) {}
};

} // namespace ccc
#endif // C4_DECLARATION_HPP
