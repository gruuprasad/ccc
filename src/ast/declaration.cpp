#include "declaration.hpp"
#include <sstream>

Declaration::Declaration(int id, ASTNode *ident, TypeSpecifier type)
    : ASTNode(id, "declaration") {
  this->ident = ident;
  this->type = type;
}

std::string Declaration::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name
     << "\" shape=box style=filled fillcolor=lightsalmon];\n";
  return ss.str();
}

InitDeclaration::InitDeclaration(int id, ASTNode *ident, TypeSpecifier type)
    : Declaration(id, ident, type) {}
