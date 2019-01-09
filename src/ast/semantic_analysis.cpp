#include "ast_node.hpp"

namespace ccc {

void ASTNode::printScopes(std::vector<std::unordered_set<std::string>> *scopes) {
  std::cout << std::endl;
  std::string pre;
  for (const std::unordered_set<std::string> &scope : *scopes) {
    for (const auto &kv : scope) {
      std::cout << pre << kv << std::endl;
    }
    pre += "\t";
  }
}

bool PrimaryExpression::nameAnalysis(std::vector<std::unordered_set<std::string>> *) {
  return true;
}

bool IdentifierExpression::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
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

bool UnaryExpression::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
  return expr->nameAnalysis(scopes);
}

bool BinaryExpression::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
  return leftExpr->nameAnalysis(scopes) && rightExpr->nameAnalysis(scopes);
}

bool CompoundStatement::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
  scopes->push_back(
      std::unordered_set<std::string>(std::ceil(block.size() / .75)));
  for (const auto &stat : block) {
    if (!stat->nameAnalysis(scopes))
      return false;
  }
  scopes->pop_back();
  return true;
}
bool ExpressionStatement::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
  return expr->nameAnalysis(scopes);
}

bool DeclarationStatement::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
  scopes->back().insert(type->getIdentifier());
  printScopes(scopes);
  return true;
}

bool TranslationUnit::nameAnalysis(std::vector<std::unordered_set<std::string>> *scopes) {
  for (const auto &child : children) {
    if (!child->nameAnalysis(scopes))
      return false;
  }
  return true;
}
void Statement::printTypes(
    std::vector<std::unordered_map<std::string,
                                   std::unique_ptr<TypeExpression>>> *scopes) {
  std::cout << std::endl;
  std::string pre;
  for (const std::unordered_map<std::string, std::unique_ptr<TypeExpression>>
           &scope : *scopes) {
    for (const auto &kv : scope) {
      std::cout << pre << kv.first << " : " << kv.second->prettyPrint(0)
        << std::endl;
    }
    pre += "\t";
  }
}
bool Statement::typeAnalysis(
    std::vector<
        std::unordered_map<std::string, std::unique_ptr<TypeExpression>>> *) {
  return true;
}

bool CompoundStatement::typeAnalysis(
    std::vector<std::unordered_map<std::string,
                                   std::unique_ptr<TypeExpression>>> *scopes) {
  scopes->push_back(
      std::unordered_map<std::string, std::unique_ptr<TypeExpression>>(
          std::ceil(block.size() / .75)));
  for (const auto &stat : block) {
    if (!stat->typeAnalysis(scopes))
      return false;
  }
  scopes->pop_back();
  return true;
  }

  bool DeclarationStatement::typeAnalysis(
      std::vector<std::unordered_map<std::string,
                                     std::unique_ptr<TypeExpression>>> *types) {
    types->back()[type->getIdentifier()] = std::move(type);
    printTypes(types);
    return true;
  }

  bool TranslationUnit::typeAnalysis(
      std::vector<std::unordered_map<std::string,
                                     std::unique_ptr<TypeExpression>>> *types) {
    for (const auto &child : children) {
      if (!child->typeAnalysis(types))
        return false;
    }
    return true;
}


bool TranslationUnit::runAnalysis() {
  std::vector<std::unordered_set<std::string>> scopes = {
    std::unordered_set<std::string>(std::ceil(children.size() / .75))};
  // TODO Preallocate once unique_ptr issue solved
  std::vector<std::unordered_map<std::string, std::unique_ptr<TypeExpression>>>
      types;
  return nameAnalysis(&scopes) && typeAnalysis(&types);
}
}
