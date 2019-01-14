/*
#include "ast_node.hpp"

namespace ccc {

void ASTNode::printScopes(Scope_list_type *scopes) {
  std::cout << std::endl;
  std::string pre;
  for (const auto &scope : *scopes) {
    for (const auto &kv : scope) {
      std::cout << pre << kv << std::endl;
    }
    pre += "\t";
  }
}

bool PrimaryExpression::nameAnalysis(Scope_list_type *) { return true; }

bool Identifier::nameAnalysis(Scope_list_type *scopes) {
  printScopes(scopes);
  for (const auto &scope : (*scopes)) {
    if (scope.find(extra) != scope.end()) {
      return true;
    }
  }
  std::cerr << SEMANTIC_ERROR(token.getLine(), token.getColumn(),
                              extra + " undefined in this scope")
            << std::endl;
  return false;
}

bool UnaryExpression::nameAnalysis(Scope_list_type *scopes) {
  return expr->nameAnalysis(scopes);
}

bool BinaryExpression::nameAnalysis(Scope_list_type *scopes) {
  return leftExpr->nameAnalysis(scopes) && rightExpr->nameAnalysis(scopes);
}

bool CompoundStatement::nameAnalysis(Scope_list_type *scopes) {
  scopes->push_back(
      std::unordered_set<std::string>(std::ceil(block.size() / .75)));
  for (const auto &stat : block) {
    if (!stat->nameAnalysis(scopes))
      return false;
  }
  scopes->pop_back();
  return true;
}
bool ExpressionStatement::nameAnalysis(Scope_list_type *scopes) {
  return expr->nameAnalysis(scopes);
}

bool DeclarationStatement::nameAnalysis(Scope_list_type *scopes) {
  scopes->back().insert(type->getIdentifier());
  printScopes(scopes);
  return true;
}

bool TranslationUnit::nameAnalysis(Scope_list_type *scopes) {
  for (const auto &child : children) {
    if (!child->nameAnalysis(scopes))
      return false;
  }
  return true;
}

void Statement::printTypes(Type_list_type *scopes) {
  std::cout << std::endl;
  std::string pre;
  for (const auto &scope : *scopes) {
    for (const auto &kv : scope) {
      std::cout << pre << kv.first << " : " << kv.second->prettyPrint(0)
                << "\n";
    }
    pre += "\t";
  }
}

bool Statement::typeAnalysis(Type_list_type *) { return true; }

bool CompoundStatement::typeAnalysis(Type_list_type *scopes) {
  scopes->push_back(
      std::unordered_map<std::string, std::unique_ptr<TypeDeclaration>>(
          std::ceil(block.size() / .75)));
  for (const auto &stat : block) {
    if (!stat->typeAnalysis(scopes))
      return false;
  }
  scopes->pop_back();
  return true;
}

bool DeclarationStatement::typeAnalysis(Type_list_type *types) {
  types->back()[type->getIdentifier()] = std::move(type);
  printTypes(types);
  return true;
}

bool TranslationUnit::typeAnalysis(Type_list_type *types) {
  for (const auto &child : children) {
    if (!child->typeAnalysis(types))
      return false;
  }
  return true;
}

bool TranslationUnit::runAnalysis() {
  // TODO Preallocate once unique_ptr issue solved
  Scope_list_type scopes;
  Type_list_type types;
  return nameAnalysis(&scopes) && typeAnalysis(&types);
}
} // namespace ccc
*/
