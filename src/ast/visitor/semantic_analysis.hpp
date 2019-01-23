#include "../ast_node.hpp"

#define OPEN_SCOPE                                                             \
  dataDeclarations.emplace_back();                                             \
  structDeclarations.emplace_back();                                           \
  structDefinitions.emplace_back();

#define CLOSE_SCOPE                                                            \
  dataDeclarations.pop_back();                                                 \
  structDeclarations.pop_back();                                               \
  structDefinitions.pop_back()

#define CLEAR_SCOPE                                                            \
  dataDeclarations.clear();                                                    \
  structDeclarations.clear();                                                  \
  structDefinitions.clear()

namespace ccc {

using ASTNodeListType = std::vector<std::unique_ptr<ASTNode>>;
// scoping
using ScopeListType = std::vector<std::unordered_set<std::string>>;
using ScopeStructListType = std::vector<
    std::unordered_map<std::string, std::unordered_set<std::string>>>;
using StructSetType = std::unordered_set<std::vector<std::string>>;
// global
using IdentifierSetType = std::unordered_set<std::string>;
using IdentifierPtrListType = std::vector<std::unique_ptr<VariableName> *>;

class SemanticVisitor : public Visitor {

  IdentifierSetType identifierList;

  ScopeListType dataDeclarations;
  IdentifierSetType functionDeclarations;
  IdentifierSetType functionDefinitions;
  ScopeListType structDeclarations;
  ScopeStructListType structDefinitions;
  // global declarations
  // declared structs
  // defined structs
  // declared functions
  // defined functions
  IdentifierSetType labels;
  IdentifierPtrListType uLabels;
  std::string error;
  int loop;

  std::vector<std::string> pre = {"glob"};
  std::string prefix(std::string s) {
    std::string p;
    for (const auto &i : pre)
      p += i + ".";
    return p + s;
  }

  void printScopes() {
    std::cout << "[";
    for (const auto &s : identifierList)
      std::cout << s << ", ";
    std::cout << "]" << std::endl;
  }

public:
  SemanticVisitor() : loop(0) { OPEN_SCOPE; }
  ~SemanticVisitor() override { CLOSE_SCOPE; }

  bool fail() {
    if (!error.empty())
      return true;
    if (!uLabels.empty()) {
      error = SEMANTIC_ERROR((*uLabels[0])->getTokenRef().getLine(),
                             (*uLabels[0])->getTokenRef().getColumn(),
                             "Use of undeclared label '" + (*uLabels[0])->name +
                                 "'");
      return true;
    }
    //    if (!uFunctionDeclarations.empty()) {
    //      error =
    //          SEMANTIC_ERROR((*uFunctionDeclarations[0])->getTokenRef().getLine(),
    //                         (*uFunctionDeclarations[0])->getTokenRef().getColumn(),
    //                         "Use of undefined method '" +
    //                             (*uFunctionDeclarations[0])->name +
    //                             "'");
    //      return true;
    //    }
    return false;
  }

  std::string getError() { return error; }

  std::string visitTranslationUnit(TranslationUnit *v) override {
    std::vector<std::string> scope = {"a"};
    //    structDefinitions.emplace_back(std::unordered_set<std::vector<std::string>>{
    //        std::vector<std::string>()});
    for (const auto &child : v->extern_list) {
      error = child->accept(this);
      if (!error.empty())
        break;
    }
    printScopes();
    //    structDefinitions.pop_back();
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    OPEN_SCOPE;
    error = v->fn_name->accept(this);
    if (!error.empty())
      return error;
    const auto &identifier = *v->fn_name->getIdentifier();
    if (functionDefinitions.find(identifier->name) != functionDefinitions.end())
      return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                            identifier->getTokenRef().getColumn(),
                            "Redefinition of '" + identifier->name + "'");
    //    for (size_t i = 0; i < uFunctionDeclarations.size(); i++)
    //      if (name == (*uFunctionDeclarations.at(i))->name)
    //        uFunctionDeclarations.erase(uFunctionDeclarations.begin() +
    //        i);
    functionDeclarations.insert(identifier->name);
    functionDefinitions.insert(identifier->name);
    error = v->fn_body->accept(this);
    printScopes();
    CLOSE_SCOPE;
    return error;
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    const auto &identifier = *v->fn_name->getIdentifier();
    //    if (functionDeclarations.find(
    //            *v->fn_name->getIdentifier()->name) ==
    //        functionDeclarations.end())
    //      uFunctionDeclarations.push_back(v->fn_name->getIdentifier());
    functionDeclarations.insert(identifier->name);
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    if (v->data_name) {
      const auto &identifier = *v->data_name->getIdentifier();
      std::string name = prefix(identifier->name);
      if (dataDeclarations.size() > 1 &&
          dataDeclarations.back().find(name) != dataDeclarations.back().end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name + "'");
      dataDeclarations.back().insert(name);
      identifierList.insert(prefix(name));
    }
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    error = v->struct_type->accept(this);
    if (!error.empty())
      return error;
    if (v->struct_alias) {
      const auto &identifier = *v->struct_alias->getIdentifier();
      if (dataDeclarations.size() > 1 &&
          dataDeclarations.back().find(identifier->name) !=
              dataDeclarations.back().end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name +
                                  "' with a different type '?' vs 'struct ?'");
      dataDeclarations.back().insert(identifier->name);
      printScopes();
    }
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    if (v->param_name) {
      const auto &identifier = *v->param_name->getIdentifier();
      if (dataDeclarations.back().find(identifier->name) !=
          dataDeclarations.back().end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name + "'");
      dataDeclarations.back().insert(identifier->name);
    }
    return error;
  }

  struct S {
    void f(){};
  } s;

  std::string visitScalarType(ScalarType *) override { return error; }

  std::string visitStructType(StructType *v) override {
    if (!v->member_list.empty()) {
      if (structDefinitions.back().find(v->struct_name) !=
          structDefinitions.back().end())
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn() + 7,
                              "Redefinition of '" + v->struct_name + "'");
      structDeclarations.back().insert(v->struct_name);
      pre.push_back(v->struct_name);
      for (const auto &d : v->member_list)
        d->accept(this);
      pre.pop_back();
    } else {
      structDeclarations.back().insert(v->struct_name);
    }
    return error;
  }

  std::string visitDirectDeclarator(DirectDeclarator *) override {
    return error;
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *) override {
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
    OPEN_SCOPE;
    if (dataDeclarations.size() == 3)
      for (const auto &s : dataDeclarations[1])
        dataDeclarations.back().insert(s);
    for (const auto &stat : v->block_items) {
      error = stat->accept(this);
      if (!error.empty())
        break;
    }
    printScopes();
    CLOSE_SCOPE;
    return error;
  }

  std::string visitIfElse(IfElse *v) override {
    error = v->condition->accept(this);
    if (!error.empty())
      return error;
    OPEN_SCOPE;
    error = v->ifStmt->accept(this);
    if (!error.empty())
      return error;
    if (v->elseStmt) {
      CLEAR_SCOPE;
      error = v->elseStmt->accept(this);
    }
    printScopes();
    CLOSE_SCOPE;
    return error;
  }

  std::string visitLabel(Label *v) override {
    auto label = v->label_name->name;
    for (const auto &l : labels) {
      if (l == label) {
        return SEMANTIC_ERROR(v->label_name->getTokenRef().getLine(),
                              v->label_name->getTokenRef().getColumn(),
                              "Redefinition of label '" + label + "'");
      }
    }
    labels.insert(label);
    for (size_t i = 0; i < uLabels.size(); i++)
      if (label == (*uLabels.at(i))->name)
        uLabels.erase(uLabels.begin() + i);
    return v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    OPEN_SCOPE;
    loop++;
    error = v->block->accept(this);
    loop--;
    printScopes();
    CLOSE_SCOPE;
    return error;
  }

  std::string visitGoto(Goto *v) override {
    auto label = v->label_name->name;
    for (const auto &l : labels)
      if (l == label)
        return error;
    uLabels.push_back(&v->label_name);
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

  std::string visitNumber(Number *) override { return error; }

  std::string visitCharacter(Character *) override { return error; }

  std::string visitString(String *) override { return error; }

  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    return v->struct_name->accept(this);
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *) override {
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
}; // namespace ccc

} // namespace ccc
