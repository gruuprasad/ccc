#ifndef C4_DECLARATION_HPP
#define C4_DECLARATION_HPP

#include "ast_node.hpp"
#include "type_specifier.hpp"

class Declaration : public ASTNode {
private:
  ASTNode *ident;
  TypeSpecifier type;
  std::string toGraphWalker() override;

public:
  Declaration(int, ASTNode *, TypeSpecifier);
};

class InitDeclaration : Declaration {
public:
  InitDeclaration(int, ASTNode *, TypeSpecifier);
};

#endif // C4_DECLARATION_HPP
