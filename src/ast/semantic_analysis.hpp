#include "ast_node.hpp"

namespace ccc {

using ASTNodeListType = std::vector<std::unique_ptr<ASTNode>>;
using ScopeListType = std::vector<std::unordered_set<std::string>>;
using IdentifierSetType = std::unordered_set<std::string>;
using ExpressionPtrListType = std::vector<std::unique_ptr<Expression> *>;
using IdentifierPtrListType = std::vector<std::unique_ptr<VariableName> *>;

class SemanticVisitor : public Visitor {

  ScopeListType dataDeclarations;
  IdentifierSetType functionDeclarations;
  IdentifierPtrListType uFunctionDeclarations;
  // global declarations
  // declared structs
  // defined structs
  // declared functions
  // defined functions
  IdentifierSetType labels;
  ExpressionPtrListType uLabels;
  std::string error;
  int loop;

  void printScopes() {
    //    std::cout << "{";
    //    for (auto i : functionDeclarations)
    //      std::cout << i << ", ";
    //    std::cout << "}\\" << uFunctionDeclarations.size();
    //    std::cout << " {";
    //    for (auto i : labels)
    //      std::cout << i << ", ";
    //    std::cout << "}\\" << uLabels.size();
    //    for (auto s : dataDeclarations) {
    //      std::cout << " [";
    //      for (auto i : s)
    //        std::cout << i << ", ";
    //      std::cout << "]";
    //    }
    //    std::cout << std::endl;
  }

public:
  SemanticVisitor() : loop(0) {}
  ~SemanticVisitor() override = default;

  bool fail() {
    if (!error.empty())
      return true;
    if (!uLabels.empty()) {
      error = SEMANTIC_ERROR((*uLabels[0])->getTokenRef().getLine(),
                             (*uLabels[0])->getTokenRef().getColumn(),
                             "Use of undeclared label '" +
                                 (*uLabels[0])->prettyPrint(0) + "'");
      return true;
    }
    //    if (!uFunctionDeclarations.empty()) {
    //      error =
    //          SEMANTIC_ERROR((*uFunctionDeclarations[0])->getTokenRef().getLine(),
    //                         (*uFunctionDeclarations[0])->getTokenRef().getColumn(),
    //                         "Use of undefined method '" +
    //                             (*uFunctionDeclarations[0])->prettyPrint(0) +
    //                             "'");
    //      return true;
    //    }
    return false;
  }

  std::string getError() { return error; }

  std::string visitTranslationUnit(TranslationUnit *v) override {
    dataDeclarations.emplace_back();
    for (const auto &child : v->extern_list) {
      error = child->accept(this);
      if (!error.empty())
        break;
    }
    printScopes();
    dataDeclarations.pop_back();
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    dataDeclarations.emplace_back();
    error = v->fn_name->accept(this);
    if (!error.empty())
      return error;
    auto name = (*v->fn_name->getIdentifier())->prettyPrint(0);
    for (size_t i = 0; i < uFunctionDeclarations.size(); i++)
      if (name == (*uFunctionDeclarations.at(i))->prettyPrint(0))
        uFunctionDeclarations.erase(uFunctionDeclarations.begin() + i);
    functionDeclarations.insert(name);
    error = v->fn_body->accept(this);
    printScopes();
    dataDeclarations.pop_back();
    return error;
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    auto name = (*v->fn_name->getIdentifier())->prettyPrint(0);
    //    if (functionDeclarations.find(
    //            (*v->fn_name->getIdentifier())->prettyPrint(0)) ==
    //        functionDeclarations.end())
    //      uFunctionDeclarations.push_back(v->fn_name->getIdentifier());
    functionDeclarations.insert(name);
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    if (v->data_name) {
      const auto &identifier = *(v->data_name->getIdentifier());
      std::string name = identifier->prettyPrint(0);
      if (dataDeclarations.size() > 1 &&
          dataDeclarations.back().find(name) != dataDeclarations.back().end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + name + "'");
      dataDeclarations.back().insert(name);
    }
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    if (v->struct_alias) {
      const auto &identifier = *(v->struct_alias->getIdentifier());
      std::string name = identifier->prettyPrint(0);
      if (dataDeclarations.size() > 1 &&
          dataDeclarations.back().find(name) != dataDeclarations.back().end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + name +
                                  "' with a different type '?' vs 'struct ?'");
      dataDeclarations.back().insert(name);
      printScopes();
    }
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    if (v->param_name) {
      const auto &identifier = *(v->param_name->getIdentifier());
      std::string name = identifier->prettyPrint(0);
      if (dataDeclarations.back().find(name) != dataDeclarations.back().end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + name + "'");
      dataDeclarations.back().insert(name);
    }
    return error;
  }

  std::string visitScalarType(ScalarType *v) override { return error; }

  std::string visitStructType(StructType *v) override { return error; }

  std::string visitDirectDeclarator(DirectDeclarator *v) override {
    return error;
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    return error;
  }

  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    return v->identifier->accept(this);
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    for (const auto &p : v->param_list) {
      error = p->accept(this);
      if (!error.empty())
        return error;
    }
    return error;
  }

  std::string visitCompoundStmt(CompoundStmt *v) override {
    dataDeclarations.emplace_back();
    if (dataDeclarations.size() == 3)
      for (auto s : dataDeclarations[1])
        dataDeclarations.back().insert(s);
    for (const auto &stat : v->block_items) {
      error = stat->accept(this);
      if (!error.empty())
        break;
    }
    printScopes();
    dataDeclarations.pop_back();
    return error;
  }

  std::string visitIfElse(IfElse *v) override {
    error = v->condition->accept(this);
    if (!error.empty())
      return error;
    dataDeclarations.emplace_back();
    error = v->ifStmt->accept(this);
    if (!error.empty())
      return error;
    if (v->elseStmt) {
      dataDeclarations.back().clear();
      error = v->elseStmt->accept(this);
    }
    printScopes();
    dataDeclarations.pop_back();
    return error;
  }

  std::string visitLabel(Label *v) override {
    auto label = v->label_name->prettyPrint(0);
    for (auto l : labels) {
      if (l == label) {
        return SEMANTIC_ERROR(v->label_name->getTokenRef().getLine(),
                              v->label_name->getTokenRef().getColumn(),
                              "Redefinition of label '" + label + "'");
      }
    }
    labels.insert(label);
    for (size_t i = 0; i < uLabels.size(); i++)
      if (label == (*uLabels.at(i))->prettyPrint(0))
        uLabels.erase(uLabels.begin() + i);
    return v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    dataDeclarations.emplace_back();
    loop++;
    error = v->block->accept(this);
    loop--;
    printScopes();
    dataDeclarations.pop_back();
    return error;
  }

  std::string visitGoto(Goto *v) override {
    auto label = v->label_name->prettyPrint(0);
    for (const auto &l : labels)
      if (l == label)
        return error;
    uLabels.push_back(v->get_label_name());
    return error;
  }

  std::string visitExpressionStmt(ExpressionStmt *v) override {
    if (v->expr)
      return v->expr->accept(this);
    return error;
  }

  std::string visitBreak(Break *v) override {
    if (loop <= 0)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "'break' statement not in a loop statement");
    return error;
  }

  std::string visitReturn(Return *v) override {
    if (v->expr)
      return v->expr->accept(this);
    return error;
  }

  std::string visitContinue(Continue *v) override {
    if (loop <= 0)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "'continue' statement not in a loop statement");
    return error;
  }

  std::string visitVariableName(VariableName *v) override {
    for (auto s : dataDeclarations)
      if (s.find(v->name) != s.end())
        return error;
    if (functionDeclarations.find(v->name) != functionDeclarations.end())
      return error;
    return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                          v->getTokenRef().getColumn(),
                          "Use of undeclared identifier '" + v->name + "'");
  }

  std::string visitNumber(Number *v) override { return error; }

  std::string visitCharacter(Character *v) override { return error; }

  std::string visitString(String *v) override { return error; }

  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    return v->struct_name->accept(this);
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    return error;
  }

  std::string visitFunctionCall(FunctionCall *v) override {
    error = v->callee_name->accept(this);
    if (!error.empty())
      return error;
    for (const auto &p : v->callee_args) {
      error = p->accept(this);
      if (!error.empty())
        return error;
    }
    return error;
  }

  std::string visitUnary(Unary *v) override { return v->operand->accept(this); }

  std::string visitSizeOf(SizeOf *v) override {
    if (v->operand)
      return v->operand->accept(this);
    return error;
  }

  std::string visitBinary(Binary *v) override {
    error = v->left_operand->accept(this);
    if (!error.empty())
      return error;
    return v->right_operand->accept(this);
  }

  std::string visitTernary(Ternary *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    error = v->left_branch->accept(this);
    if (!error.empty())
      return error;
    return v->right_branch->accept(this);
  }

  std::string visitAssignment(Assignment *v) override {
    error = v->left_operand->accept(this);
    if (!error.empty())
      return error;
    return v->right_operand->accept(this);
  }
};

} // namespace ccc
