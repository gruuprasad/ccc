#include "../ast_node.hpp"

namespace ccc {

using IdentifierSetType = std::unordered_set<std::string>;
using IdentifierMapType = std::unordered_map<std::string, std::string>;

class SemanticVisitor : public Visitor {

  IdentifierSetType definitions;
  IdentifierSetType declarations;

  IdentifierPtrListType uLabels;
  std::string error;
  int loop;

  std::vector<std::string> pre = {"$"};
  std::string prefix(const std::string &s = "") {
    std::stringstream ss;
    for (const auto &i : pre)
      ss << i << ".";
    return ss.str() + s;
  }
  std::string prefix(const std::vector<std::string> &pre) {
    std::stringstream ss;
    for (const auto &i : pre)
      ss << i << ".";
    return ss.str();
  }

  void printScopes() {
    std::cout << "[";
    for (const auto &s : declarations)
      std::cout << s << ", ";
    std::cout << "] [";
    for (const auto &s : definitions)
      std::cout << s << ", ";
    std::cout << "]" << std::endl;
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
                             "Use of undeclared label '" + (*uLabels[0])->name +
                                 "'");
      return true;
    }
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
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    const auto &identifier = *v->fn_name->getIdentifier();
    auto name = prefix(identifier->name);
    pre.emplace_back(identifier->name);
    error = v->fn_name->accept(this);
    if (!error.empty())
      return error;
    if (definitions.find(name) != definitions.end())
      return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                            identifier->getTokenRef().getColumn(),
                            "Redefinition of '" + identifier->name + "'");
    definitions.insert(name);
    declarations.insert(name);
    error = v->fn_body->accept(this);
    // delete all nested definitions
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).find(prefix("$")) == 0)
        declarations.erase(it++);
      else
        ++it;
    pre.pop_back();
    return error;
  }
  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    const auto &identifier = *v->fn_name->getIdentifier();
    auto name = prefix(identifier->name);
    declarations.insert(name);
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    if (v->data_name) {
      const auto &identifier = *v->data_name->getIdentifier();
      std::string name = prefix(identifier->name);

      if (prefix() != "$." && declarations.find(name) != declarations.end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name + "'");
      declarations.insert(name);
    }
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    error = v->struct_type->accept(this);
    if (!error.empty())
      return error;
    if (v->struct_alias) {
      const auto &identifier = *v->struct_alias->getIdentifier();
      std::string name = prefix(identifier->name);
      if (declarations.find(name) != declarations.end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name +
                                  "' with a different type '?' vs 'struct ?'");
      declarations.insert(name);
    }
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    if (v->param_name) {
      const auto &identifier = *v->param_name->getIdentifier();
      std::string name = prefix(identifier->name);
      if (declarations.find(name) != declarations.end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name + "'");
      declarations.insert(name);
    }
    return error;
  }

  std::string visitScalarType(ScalarType *) override { return error; }

  std::string visitStructType(StructType *v) override {
    auto name = prefix(v->struct_name);
    if (!v->member_list.empty()) {
      if (definitions.find(name) != definitions.end())
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn() + 7,
                              "Redefinition of '" + v->struct_name + "'");
      pre.push_back(v->struct_name);
      for (const auto &d : v->member_list) {
        error = d->accept(this);
        if (!error.empty())
          return error;
      }
      pre.pop_back();
      definitions.insert(name);
    }
    declarations.insert(name);
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
    pre.emplace_back("$");
    for (const auto &p : v->param_list) {
      error = p->accept(this);
      if (!error.empty())
        return error;
    }
    pre.pop_back();
    return error;
  }

  std::string visitCompoundStmt(CompoundStmt *v) override {
    pre.emplace_back("$");
    for (const auto &stat : v->block_items) {
      error = stat->accept(this);
      if (!error.empty())
        break;
    }
    // delete all nested definitions
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).find(prefix()) == 0)
        declarations.erase(it++);
      else
        ++it;
    for (auto it = definitions.begin(); it != definitions.end();)
      if ((*it).find(prefix()) == 0)
        definitions.erase(it++);
      else
        ++it;
    pre.pop_back();
    return error;
  }

  std::string visitIfElse(IfElse *v) override {
    error = v->condition->accept(this);
    if (!error.empty())
      return error;
    pre.emplace_back("if");
    error = v->ifStmt->accept(this);
    if (!error.empty())
      return error;
    if (v->elseStmt) {
      pre.emplace_back("else");
      error = v->elseStmt->accept(this);
      pre.pop_back();
    }
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).find(prefix()) == 0)
        declarations.erase(it++);
      else
        ++it;
    for (auto it = definitions.begin(); it != definitions.end();)
      if ((*it).find(prefix()) == 0)
        definitions.erase(it++);
      else
        ++it;
    pre.pop_back();
    return error;
  }

  std::string visitLabel(Label *v) override {
    auto label = v->label_name->name;
    for (const auto &l : definitions) {
      if (l == label) {
        return SEMANTIC_ERROR(v->label_name->getTokenRef().getLine(),
                              v->label_name->getTokenRef().getColumn(),
                              "Redefinition of label '" + label + "'");
      }
    }
    definitions.insert(label);
    for (size_t i = 0; i < uLabels.size(); i++)
      if (label == (*uLabels.at(i))->name)
        uLabels.erase(uLabels.begin() + i);
    return v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;

    loop++;
    pre.emplace_back("while");
    error = v->block->accept(this);
    loop--;
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).find(prefix()) == 0)
        declarations.erase(it++);
      else
        ++it;
    for (auto it = definitions.begin(); it != definitions.end();)
      if ((*it).find(prefix()) == 0)
        definitions.erase(it++);
      else
        ++it;
    pre.pop_back();
    return error;
  }

  std::string visitGoto(Goto *v) override {
    auto label = v->label_name->name;
    for (const auto &l : definitions)
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
    std::string name = prefix(v->name);
    if (declarations.find(name) != declarations.end())
      return error;
    std::string p;
    for (const auto &s : pre) {
      p += s + ".";
      if (declarations.find(p + v->name) != declarations.end())
        return error;
    }
    return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                          v->getTokenRef().getColumn(),
                          "Use of undeclared identifier '" + name + "'");
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
