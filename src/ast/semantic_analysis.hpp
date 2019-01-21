#include "ast_node.hpp"

namespace ccc {

using ASTNodeListType = std::vector<std::unique_ptr<ASTNode>>;
using ScopeListType = std::vector<std::unordered_set<std::string>>;
using LabelSetType = std::unordered_set<std::string>;
using LabelListType = std::vector<std::unique_ptr<Expression> *>;

class SemanticVisitor : public Visitor {
  ScopeListType scopes;
  LabelSetType labels;
  LabelListType ulabels;
  std::string error;
  int loop;

public:
  SemanticVisitor() : loop(0) {}
  ~SemanticVisitor() override = default;

  bool fail() {
    if (!error.empty())
      return true;
    if (!ulabels.empty()) {
      error = SEMANTIC_ERROR((*ulabels[0])->getTokenRef().getLine(),
                             (*ulabels[0])->getTokenRef().getColumn(),
                             "Use of undeclared label '" +
                                 (*ulabels[0])->prettyPrint(0) + "'");
      return true;
    }
    return false;
  }

  std::string getError() { return error; }

  std::string visitTranslationUnit(TranslationUnit *v) override {
    scopes.emplace_back(v->extern_list.size() * 2);
    for (const auto &child : v->extern_list) {
      error = child->accept(this);
      if (!error.empty())
        break;
    }
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    return v->fn_body->accept(this);
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
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
    return error;
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    return error;
  }

  std::string visitCompoundStmt(CompoundStmt *v) override {
    scopes.emplace_back(v->block_items.size() * 2);
    for (const auto &stat : v->block_items) {
      error = stat->accept(this);
      if (!error.empty())
        break;
    }
    scopes.pop_back();
    return error;
  }

  std::string visitIfElse(IfElse *v) override {
    error = v->condition->accept(this);
    if (!error.empty())
      return error;
    scopes.emplace_back();
    error = v->ifStmt->accept(this);
    if (!error.empty())
      return error;
    if (v->elseStmt) {
      scopes.back().clear();
      error = v->elseStmt->accept(this);
    }
    scopes.pop_back();
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
    for (size_t i = 0; i < ulabels.size(); i++)
      if (label == (*ulabels.at(i))->prettyPrint(0))
        ulabels.erase(ulabels.begin() + i);
    return v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    scopes.emplace_back();
    loop++;
    error = v->block->accept(this);
    loop--;
    scopes.pop_back();
    return error;
  }

  std::string visitGoto(Goto *v) override {
    auto label = v->label_name->prettyPrint(0);
    for (auto l : labels)
      if (l == label)
        return error;
    ulabels.push_back(v->get_label_name());
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

  std::string visitVariableName(VariableName *v) override { return error; }

  std::string visitNumber(Number *v) override { return error; }

  std::string visitCharacter(Character *v) override { return error; }

  std::string visitString(String *v) override { return error; }

  std::string visitMemberAccessOp(MemberAccessOp *v) override { return error; }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    return error;
  }

  std::string visitFunctionCall(FunctionCall *v) override { return error; }

  std::string visitUnary(Unary *v) override { return error; }

  std::string visitSizeOf(SizeOf *v) override { return error; }

  std::string visitBinary(Binary *v) override { return error; }

  std::string visitTernary(Ternary *v) override { return error; }

  std::string visitAssignment(Assignment *v) override { return error; }
};

} // namespace ccc
