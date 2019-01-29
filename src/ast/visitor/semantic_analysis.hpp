#include "../ast_node.hpp"
#include <iterator>
#include <sstream>

namespace ccc {

using IdentifierSetType = std::unordered_set<std::string>;
using IdentifierMapType =
    std::unordered_map<std::string, std::shared_ptr<RawType>>;

class SemanticVisitor : public Visitor {

  IdentifierSetType definitions;
  IdentifierMapType declarations;
  IdentifierSetType labels;
  IdentifierPtrListType uLabels;
  std::string error;
  int loop_counter;
  bool global_scope;
  bool temporary = true;

  std::shared_ptr<RawType> raw_type = nullptr;
  std::shared_ptr<RawType> jump_type = nullptr;

  std::vector<std::string> pre = {"$"};

  std::string prefix() {
    std::ostringstream os;
    std::copy(pre.begin(), pre.end(),
              std::ostream_iterator<std::string>(os, "."));
    return os.str();
  }

  std::string prefix(const std::string &s) { return prefix() + s; }

public:
  SemanticVisitor() : loop_counter(0), global_scope(false) {}
  ~SemanticVisitor() override = default;

  void printScopes() {
    std::stringstream ss;
    for (const auto &d : declarations)
      ss << "  " << d.first << ":"
         << "\033[31;m" << d.second->print() << "\033[0;m,\n";
    std::cout << "[\n" << ss.str() << "]";
    //    std::ostringstream os;
    //    std::copy(definitions.begin(), definitions.end(),
    //              std::ostream_iterator<std::string>(os, ", "));
    //        std::cout << "{" << os.str() << "}";
    std::cout << std::endl;
  }

  bool fail() {
    if (!error.empty())
      return true;
    for (const auto &l : uLabels) {
      if (labels.find((*l)->name) == labels.end()) {
        error = SEMANTIC_ERROR((*l)->getTokenRef().getLine(),
                               (*l)->getTokenRef().getColumn(),
                               "Use of undeclared label '" + (*l)->name + "'");
        return true;
      }
    }
    return false;
  }

  std::string getError() { return error; }

  std::string visitTranslationUnit(TranslationUnit *v) override {
    //    std::vector<std::string> scope = {"a"};
    //    structDefinitions.emplace_back(std::unordered_set<std::vector<std::string>>{
    //        std::vector<std::string>()});
    for (const auto &child : v->extern_list) {
      global_scope = true;
      error = child->accept(this);
      if (!error.empty())
        break;
    }
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    global_scope = false;
    v->return_type->accept(this);
    if (v->fn_name->getIdentifier() != nullptr) {
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
      if (declarations.find(name) != declarations.end()) {
        if (!declarations[name]->compare_equal(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
      jump_type = raw_type->get_return();
    } else {
      v->fn_name->accept(this);
      jump_type = raw_type->get_return();
    }
    error = v->fn_body->accept(this);
    // delete all nested definitions
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).first.compare(0, prefix("$").size(), prefix("$")) == 0)
        declarations.erase(it++);
      else
        ++it;
    pre.pop_back();
    return error;
  }
  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    v->return_type->accept(this);
    if (v->fn_name->getIdentifier() != nullptr) {
      const auto &identifier = *v->fn_name->getIdentifier();
      auto name = prefix(identifier->name);
      v->fn_name->accept(this);
      if (declarations.find(name) != declarations.end()) {
        if (!declarations[name]->compare_equal(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
    }
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    v->data_type->accept(this);
    if (v->data_name) {
      const auto &identifier = *v->data_name->getIdentifier();
      std::string name = prefix(identifier->name);
      v->data_name->accept(this);
      if (declarations.find(name) != declarations.end()) {
        if (!global_scope)
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name + "'");
        else if (!declarations[name]->compare_equal(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
    }
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    error = v->struct_type->accept(this);
    if (!error.empty())
      return error;
    if (v->struct_alias) {
      global_scope = false;
      const auto &identifier = *v->struct_alias->getIdentifier();
      std::string name = prefix(identifier->name);
      v->struct_alias->accept(this);
      if (declarations.find(name) != declarations.end()) {
        if (!global_scope)
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name + "'");
        else if (!declarations[name]->compare_equal(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
      if (!(*v->struct_type->getStructType()).member_list.empty()) {
        pre.emplace_back(identifier->name);
        for (const auto &d : (*v->struct_type->getStructType()).member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
        }
        pre.pop_back();
      }
    } else {
      if (!(*v->struct_type->getStructType()).struct_name &&
          !(*v->struct_type->getStructType()).member_list.empty()) {
        for (const auto &d : (*v->struct_type->getStructType()).member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
        }
      }
    }
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    v->param_type->accept(this);
    if (v->param_name && v->param_name->getIdentifier() != nullptr) {
      const auto &identifier = *v->param_name->getIdentifier();
      std::string name = prefix(identifier->name);
      if (declarations.find(name) != declarations.end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name + "'");
      v->param_name->accept(this);
      declarations[name] = raw_type;
    }
    return error;
  }

  std::string visitScalarType(ScalarType *v) override {
    switch (v->type_kind) {
    case ScalarTypeValue::VOID:
      raw_type = make_unique<RawScalarType>(RawTypeValue::VOID);
      break;
    case ScalarTypeValue::INT:
      raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
      break;
    case ScalarTypeValue::CHAR:
      raw_type = make_unique<RawScalarType>(RawTypeValue::CHAR);
    }
    return error;
  }

  std::string visitStructType(StructType *v) override {
    if (v->struct_name) {
      global_scope = false;
      auto name = "@" + v->struct_name->name;
      if (prefix().length() > name.length() &&
          0 == prefix().compare(prefix().length() - name.length() - 1,
                                name.length(), name))
        return SEMANTIC_ERROR(v->struct_name->getTokenRef().getLine(),
                              v->struct_name->getTokenRef().getColumn(),
                              "Member '" + v->struct_name->name +
                                  "' has the same name as its class");
      name = prefix("@" + v->struct_name->name);
      if (v->members) {
        if (definitions.find(name) != definitions.end())
          return SEMANTIC_ERROR(v->struct_name->getTokenRef().getLine(),
                                v->struct_name->getTokenRef().getColumn(),
                                "Redefinition of '" + v->struct_name->name +
                                    "'");
        pre.push_back("@" + v->struct_name->name);
        for (const auto &d : v->member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
        }
        pre.pop_back();
        definitions.insert(name);
      }
      raw_type = make_unique<RawStructType>("struct " + v->struct_name->name);
      declarations[name] = raw_type;
    } else
      raw_type = make_unique<RawStructType>("struct");
    return error;
  }

  std::string visitDirectDeclarator(DirectDeclarator *) override {
    // break recursive raw type generation
    return error;
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    for (unsigned int i = 0; i < v->pointerCount; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
    return error;
  }

  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    v->identifier->accept(this);
    for (int i = 0; i < v->indirection_level; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
    return error;
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    pre.emplace_back("$");
    v->identifier->accept(this);
    auto return_type = raw_type;
    auto tmp = std::vector<std::shared_ptr<RawType>>();
    for (const auto &p : v->param_list) {
      error = p->accept(this);
      if (!error.empty())
        return error;
      tmp.emplace_back(raw_type);
    }
    pre.pop_back();
    raw_type = make_unique<RawFunctionType>(return_type, tmp);
    return v->return_ptr->accept(this);
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
      if ((*it).first.compare(0, prefix().size(), prefix()) == 0)
        declarations.erase(it++);
      else
        ++it;
    for (auto it = definitions.begin(); it != definitions.end();)
      if ((*it).compare(0, prefix().size(), prefix()) == 0)
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
    if (raw_type->getRawTypeValue() != RawTypeValue::INT) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Condition has to be int, found " + raw_type->print());
    }
    pre.emplace_back("if");
    error = v->ifStmt->accept(this);
    if (!error.empty())
      return error;
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).first.compare(0, prefix().size(), prefix()) == 0)
        declarations.erase(it++);
      else
        ++it;
    for (auto it = definitions.begin(); it != definitions.end();)
      if ((*it).compare(0, prefix().size(), prefix()) == 0)
        definitions.erase(it++);
      else
        ++it;
    pre.pop_back();
    if (v->elseStmt) {
      pre.emplace_back("else");
      error = v->elseStmt->accept(this);
      if (!error.empty())
        return error;
      for (auto it = declarations.begin(); it != declarations.end();)
        if ((*it).first.compare(0, prefix().size(), prefix()) == 0)
          declarations.erase(it++);
        else
          ++it;
      for (auto it = definitions.begin(); it != definitions.end();)
        if ((*it).compare(0, prefix().size(), prefix()) == 0)
          definitions.erase(it++);
        else
          ++it;
      pre.pop_back();
    }
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
    return v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    if (raw_type->getRawTypeValue() != RawTypeValue::INT) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Predicate has to be int, found " + raw_type->print());
    }
    loop_counter++;
    pre.emplace_back("while");
    error = v->block->accept(this);
    loop_counter--;
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).first.compare(0, prefix().size(), prefix()) == 0)
        declarations.erase(it++);
      else
        ++it;
    for (auto it = definitions.begin(); it != definitions.end();)
      if ((*it).compare(0, prefix().size(), prefix()) == 0)
        definitions.erase(it++);
      else
        ++it;
    pre.pop_back();
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
    if (loop_counter <= 0)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "'break' statement not in a loop statement");
    return error;
  }

  std::string visitReturn(Return *v) override {
    if (v->expr) {
      error = v->expr->accept(this);
      if (!error.empty())
        return error;
      if (!raw_type->compare_equal(jump_type))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't return " + raw_type->print() +
                                  " instead of " + jump_type->print());
      return error;
    }
    if (jump_type->getRawTypeValue() != RawTypeValue::VOID)
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't return void instead of " + jump_type->print());
    return error;
  }

  std::string visitContinue(Continue *v) override {
    if (loop_counter <= 0)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "'continue' statement not in a loop statement");
    return error;
  }

  std::string visitVariableName(VariableName *v) override {
    temporary = false;
    std::string name;
    std::string tmp_pre = prefix();
    while (tmp_pre.find('.') != std::string::npos) {
      tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
      name = tmp_pre + "." + v->name;
      if (declarations.find(name) != declarations.end()) {
        raw_type = declarations[name];
        return error;
      }
    }
    return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                          v->getTokenRef().getColumn(),
                          "Use of undeclared identifier '" + name + "'");
  }

  std::string visitNumber(Number *) override {
    temporary = true;
    raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    return error;
  }

  std::string visitCharacter(Character *) override {
    temporary = true;
    raw_type = make_unique<RawScalarType>(RawTypeValue::CHAR);
    return error;
  }

  std::string visitString(String *) override {
    temporary = true;
    raw_type = make_unique<RawPointerType>(
        make_unique<RawScalarType>(RawTypeValue::CHAR));
    return error;
  }

  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    error = v->struct_name->accept(this);
    temporary = false;
    std::string sub;
    switch (v->op_kind) {
    case PostFixOpValue::ARROW: {
      if (raw_type->getRawTypeValue() != RawTypeValue::POINTER)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't dereference " + raw_type->print());
      if (raw_type->deref()->getRawTypeValue() != RawTypeValue::STRUCT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't access member of " + raw_type->print());
      sub = raw_type->deref()->getRawStructType()->getName();
      break;
    }
    case PostFixOpValue::DOT:
      if (raw_type->getRawTypeValue() != RawTypeValue::STRUCT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't access member of " + raw_type->print());
      sub = raw_type->getRawStructType()->getName();
    }
    if (sub == "struct") {
      std::string name;
      std::string tmp_pre = prefix();
      while (tmp_pre.find('.') != std::string::npos) {
        tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
        name = tmp_pre + "." + v->struct_name->getVariableName()->name + "." +
               v->member_name->getVariableName()->name;
        if (declarations.find(name) != declarations.end()) {
          raw_type = declarations[name];
          return error;
        }
      }
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't find member " + v->member_name->getVariableName()->name +
              " of " + v->struct_name->getVariableName()->name);
    } else {
      std::string name;
      std::string tmp_pre = prefix();
      while (tmp_pre.find('.') != std::string::npos) {
        tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
        name = tmp_pre + ".@" + sub.substr(7, sub.size()) + "." +
               v->member_name->getVariableName()->name;
        if (declarations.find(name) != declarations.end()) {
          raw_type = declarations[name];
          return error;
        }
      }
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't find member " + v->member_name->getVariableName()->name +
              " of " + sub);
    }
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    error = v->array_name->accept(this);
    temporary = false;
    return error; // TODO
  }

  std::string visitFunctionCall(FunctionCall *v) override {
    error = v->callee_name->accept(this);
    if (!error.empty())
      return error;
    if (raw_type->getRawTypeValue() != RawTypeValue::FUNCTION)
      return SEMANTIC_ERROR(v->callee_name->getTokenRef().getLine(),
                            v->callee_name->getTokenRef().getColumn(),
                            "Can't call " + raw_type->print());

    auto return_type = raw_type->get_return();
    auto calle_arg_types = raw_type->get_param();

    if (calle_arg_types.size() < v->callee_args.size())
      return SEMANTIC_ERROR(v->callee_name->getTokenRef().getLine(),
                            v->callee_name->getTokenRef().getColumn(),
                            "Too many arguments for " + raw_type->print());

    for (unsigned int i = 0; i < calle_arg_types.size(); i++) {
      if (i >= v->callee_args.size())
        return SEMANTIC_ERROR(v->callee_name->getTokenRef().getLine(),
                              v->callee_name->getTokenRef().getColumn(),
                              "Too few arguments for " + raw_type->print());
      error = v->callee_args[i]->accept(this);
      if (!error.empty())
        return error;
      if (calle_arg_types[i]->getRawTypeValue() != RawTypeValue::VOID &&
          !calle_arg_types[i]->compare_equal(raw_type))
        return SEMANTIC_ERROR(v->callee_name->getTokenRef().getLine(),
                              v->callee_name->getTokenRef().getColumn(),
                              "Can't call " + calle_arg_types[i]->print() +
                                  " with " + raw_type->print());
    }
    raw_type = return_type;
    temporary = true;
    return error;
  }

  std::string visitUnary(Unary *v) override {
    temporary = true;
    error = v->operand->accept(this);
    if (!error.empty())
      return error;
    switch (v->op_kind) {
    case UnaryOpValue::MINUS:
      if (raw_type->getRawTypeValue() != RawTypeValue::INT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't minus " + raw_type->print());
      break;
    case UnaryOpValue::NOT:
      if (raw_type->getRawTypeValue() != RawTypeValue::INT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't negate " + raw_type->print());
      break;
    case UnaryOpValue::DEREFERENCE:
      if (raw_type->getRawTypeValue() != RawTypeValue::POINTER)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't dereference " + raw_type->print());
      raw_type = raw_type->deref();
      break;
    case UnaryOpValue::ADDRESS_OF:
      raw_type = make_unique<RawPointerType>(raw_type);
      if (temporary)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't get address of temporay object");
      break;
    }
    return error;
  }

  std::string visitSizeOf(SizeOf *v) override {
    if (v->operand)
      error = v->operand->accept(this);
    temporary = true;
    raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    return error;
  }

  std::string visitBinary(Binary *v) override {
    error = v->left_operand->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    error = v->right_operand->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " and " + rhs_type->print());
    }
    temporary = true;
    return error;
  }

  std::string visitTernary(Ternary *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    if (raw_type->getRawTypeValue() != RawTypeValue::INT) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Predicate has to be int, found " + raw_type->print());
    }
    error = v->left_branch->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    error = v->right_branch->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't branch with " + lhs_type->print() + " and " +
                                rhs_type->print());
    }
    temporary = true;
    return error;
  }

  std::string visitAssignment(Assignment *v) override {
    error = v->left_operand->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    error = v->right_operand->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't assign " + rhs_type->print() + " to " + lhs_type->print());
    }
    temporary = true;
    return error;
  }
};

} // namespace ccc
