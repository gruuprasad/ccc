#include "../ast_node.hpp"
#include <iterator>
#include <sstream>

namespace ccc {

using ASTNodeListType = std::vector<std::unique_ptr<ASTNode>>;
using ScopeListType = std::vector<std::unordered_set<std::string>>;
using IdentifierSetType = std::unordered_set<std::string>;
using IdentifierPtrListType = std::vector<std::unique_ptr<VariableName> *>;
using IdentifierMapType =
    std::unordered_map<std::string, std::shared_ptr<RawType>>;

class SemanticVisitor : public Visitor<std::string> {

  IdentifierSetType definitions;
  IdentifierMapType declarations;
  IdentifierSetType labels;
  IdentifierPtrListType uLabels;
  std::string error;
  int loop_counter;
  bool temporary = true;
  std::string alias;
  bool function_definition = false;

  std::shared_ptr<RawType> raw_type = nullptr;
  std::shared_ptr<RawType> jump_type = nullptr;

  std::vector<std::string> pre;

  std::string prefix() {
    std::ostringstream os;
    std::copy(pre.begin(), pre.end(),
              std::ostream_iterator<std::string>(os, "."));
    return os.str();
  }

  std::string prefix(const std::string &s) { return prefix() + s; }

public:
  SemanticVisitor() : loop_counter(0), pre({"$"}) {}
  ~SemanticVisitor() override = default;

  void printScopes() {
    std::stringstream ss;
    for (const auto &d : declarations)
      ss << "  " << d.first << ":"
         << "\033[31;m" << d.second->print() << "\033[0;m,\n";
    std::cout << "[\n" << ss.str() << "]";
    std::ostringstream os;
    std::copy(definitions.begin(), definitions.end(),
              std::ostream_iterator<std::string>(os, ", "));
    std::cout << "{" << os.str() << "}";
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
    raw_type = nullptr;
    for (const auto &child : v->extern_list) {
      error = child->accept(this);
      if (!error.empty())
        break;
    }
    return error;
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    function_definition = true;
    v->return_type->accept(this);
    if (v->fn_name && v->fn_name->getIdentifier()) {
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
        if (!declarations[name]->compare_exact(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
      if (raw_type->isFunctionPointer())
        return SEMANTIC_ERROR(v->fn_name->getTokenRef().getLine(),
                              v->fn_name->getTokenRef().getColumn(),
                              "Can't define " + raw_type->print());
      v->setUType(raw_type);
      v->setUIdentifier(name);
      jump_type = raw_type->get_return();
    } else if (v->fn_name)
      return SEMANTIC_ERROR(v->fn_name->getTokenRef().getLine(),
                            v->fn_name->getTokenRef().getColumn(),
                            "Missing identifier");
    function_definition = false;
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
    function_definition = false;
    v->return_type->accept(this);
    if (v->fn_name && v->fn_name->getIdentifier()) {
      const auto &identifier = *v->fn_name->getIdentifier();
      auto name = prefix(identifier->name);
      v->fn_name->accept(this);
      if (declarations.find(name) != declarations.end()) {
        if (!declarations[name]->compare_exact(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
      v->isFuncPtr = raw_type->isFunctionPointer();
      v->setUType(raw_type);
      v->setUIdentifier(name);
    } else
      return SEMANTIC_ERROR(v->return_type->getTokenRef().getLine(),
                            v->return_type->getTokenRef().getColumn(),
                            "Declaration without declarator");
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).first.compare(0, prefix("$").size(), prefix("$")) == 0)
        declarations.erase(it++);
      else
        ++it;
    return error;
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    error = v->data_type->accept(this);
    if (!error.empty())
      return error;
    if (v->data_name && v->data_name->getIdentifier()) {
      const auto &identifier = *v->data_name->getIdentifier();
      std::string name = prefix(identifier->name);
      v->data_name->accept(this);
      if (declarations.find(name) != declarations.end()) {
        if (!(prefix() == "$."))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name + "'");
        else if (!declarations[name]->compare_exact(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
      v->global = prefix() == "$.";
      v->setUType(raw_type);
      v->setUIdentifier(name);
    } else if (raw_type->getRawTypeValue() != RawTypeValue::STRUCT) {
      return SEMANTIC_ERROR(v->data_type->getTokenRef().getLine(),
                            v->data_type->getTokenRef().getColumn(),
                            "Declaration without declarator");
    }
    if (raw_type->getRawTypeValue() == RawTypeValue::VOID && v->data_name)
      return SEMANTIC_ERROR(v->data_name->getTokenRef().getLine(),
                            v->data_name->getTokenRef().getColumn(),
                            "Incomplete type");
    return error;
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    error = v->struct_type->accept(this);
    bool anonymous = !raw_type;
    if (!error.empty())
      return error;
    if (v->struct_alias) {
      const auto &identifier = *v->struct_alias->getIdentifier();
      std::string name = prefix(identifier->name);
      if (anonymous)
        raw_type = std::make_shared<RawStructType>(
            "struct " + prefix("__" + prefix(identifier->name) + "__"));
      v->struct_alias->accept(this);
      if (declarations.find(name) != declarations.end()) {
        if (!(prefix() == "$.") || anonymous)
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name + "'");
        else if (!declarations[name]->compare_exact(raw_type))
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name +
                                    "' of type " + declarations[name]->print() +
                                    " with differtent type " +
                                    raw_type->print());
      } else
        declarations[name] = raw_type;
      if (anonymous && (*v->struct_type->getStructType()).is_definition) {
        pre.emplace_back("__" + name + "__");
        for (const auto &d : (*v->struct_type->getStructType()).member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
        }
        pre.pop_back();
      }
    } else if (anonymous) {
      for (const auto &d : (*v->struct_type->getStructType()).member_list) {
        error = d->accept(this);
        if (!error.empty())
          return error;
      }
    }
    return error;
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    if (function_definition && !v->param_name)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Expected identifier");
    auto tmp = function_definition;
    function_definition = false;
    v->param_type->accept(this);
    if (v->param_name && v->param_name->getIdentifier()) {
      const auto &identifier = *v->param_name->getIdentifier();
      std::string name = prefix(identifier->name);
      if (declarations.find(name) != declarations.end())
        return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                              identifier->getTokenRef().getColumn(),
                              "Redefinition of '" + identifier->name + "'");
      v->param_name->accept(this);
      declarations[name] = raw_type;
      (*v->param_name->getIdentifier())->setUIdentifier(name);
    } else if (v->param_name)
      v->param_name->accept(this);
    if (raw_type->getRawTypeValue() == RawTypeValue::VOID && v->param_name)
      return SEMANTIC_ERROR(v->param_name->getTokenRef().getLine(),
                            v->param_name->getTokenRef().getColumn(),
                            "Incomplete type");
    function_definition = tmp;
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
      break;
    }
    return error;
  }

  std::string visitStructType(StructType *v) override {
    if (v->struct_name) {
      raw_type = nullptr;
      if (v->is_definition) {
        std::string p = prefix();
        p = p.substr(0, p.find_first_of("__"));
        auto name = "__" + v->struct_name->name + "__";
        for (const auto &s : definitions) {
          if (s.size() > name.size()) {
            auto sub = s.substr(s.size() - name.size(), s.size());
            if (s.substr(0, p.size()) == s.substr(0, s.find_first_of("__")) &&
                sub == name) {
              return SEMANTIC_ERROR(v->struct_name->getTokenRef().getLine(),
                                    v->struct_name->getTokenRef().getColumn(),
                                    "Redefinition of 'struct " +
                                        v->struct_name->name + "'");
            }
          }
        }
      } else {
        std::string tmp;
        std::string tmp_pre = prefix();
        while (tmp_pre.find('.') != std::string::npos) {
          tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
          tmp = tmp_pre + ".__" + v->struct_name->name + "__";
          if (declarations.find(tmp) != declarations.end()) {
            raw_type = make_unique<RawStructType>("struct " + tmp);
          }
        }
      }
      if (!raw_type)
        raw_type = make_unique<RawStructType>(
            "struct " + prefix("__" + v->struct_name->name + "__"));
      auto name = prefix("__" + v->struct_name->name + "__");
      declarations[name] = raw_type;
      auto ret = raw_type;
      if (v->is_definition) {
        if (definitions.find(name) != definitions.end())
          return SEMANTIC_ERROR(v->struct_name->getTokenRef().getLine(),
                                v->struct_name->getTokenRef().getColumn(),
                                "Redefinition of 'struct " +
                                    v->struct_name->name + "'");
        definitions.insert(name);
        pre.push_back("__" + v->struct_name->name + "__");
        for (const auto &d : v->member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
        }
        pre.pop_back();
      }
      raw_type = ret;
    } else {
      raw_type = nullptr;
    }
    return error;
  }

  std::string visitAbstractType(AbstractType *v) override {
    v->type->accept(this);
    for (int i = 0; i < v->ptr_count; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
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
    if (!error.empty())
      return error;
    int lvl = 0;
    while (raw_type->getRawTypeValue() == RawTypeValue::POINTER) {
      raw_type = raw_type->deref();
      lvl++;
    }
    v->return_ptr->accept(this);
    if (!error.empty())
      return error;
    auto return_type = raw_type;
    auto tmp = std::vector<std::shared_ptr<RawType>>();
    if (v->param_list.size() == 1 && !v->param_list[0]->param_name) {
      error = v->param_list[0]->param_type->accept(this);
      if (raw_type->compare_equal(
              std::make_shared<RawScalarType>(RawTypeValue::VOID))) {
        pre.pop_back();
        raw_type = make_unique<RawFunctionType>(return_type, tmp);
        for (int i = 0; i < lvl; i++)
          raw_type = make_unique<RawPointerType>(raw_type);
        return error;
      }
    }
    if (function_definition)
      function_definition =
          return_type->getRawTypeValue() != RawTypeValue::FUNCTION;
    for (const auto &p : v->param_list) {
      error = p->accept(this);
      if (!error.empty())
        return error;
      tmp.emplace_back(raw_type);
    }
    pre.pop_back();
    if (return_type->getRawTypeValue() == RawTypeValue::FUNCTION) {
      auto tmp_ret = return_type->get_return();
      auto tmp_param = return_type->get_param();
      int lvl_p = 0;
      while (tmp_ret->getRawTypeValue() == RawTypeValue::POINTER) {
        tmp_ret = tmp_ret->deref();
        lvl_p++;
      }
      return_type = make_unique<RawFunctionType>(tmp_ret, tmp);
      for (int i = 0; i < lvl_p; i++)
        return_type = make_unique<RawPointerType>(return_type);
      raw_type = make_unique<RawFunctionType>(return_type, tmp_param);
    } else
      raw_type = make_unique<RawFunctionType>(return_type, tmp);
    for (int i = 0; i < lvl; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
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
    if (!raw_type->compare_equal(
            make_unique<RawScalarType>(RawTypeValue::INT))) {
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
    if (!raw_type->compare_equal(
            make_unique<RawScalarType>(RawTypeValue::INT))) {
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
        return SEMANTIC_ERROR(v->expr->getTokenRef().getLine(),
                              v->expr->getTokenRef().getColumn(),
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
        v->setUIdentifier(name);
        return error;
      }
    }
    return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                          v->getTokenRef().getColumn(),
                          "Use of undeclared identifier '" + name + "'");
  }

  std::string visitNumber(Number *v) override {
    temporary = v->num_value != 0;
    //    if (v->num_value < 0 || v->num_value >
    //    std::numeric_limits<int>::max())
    //      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
    //                            v->getTokenRef().getColumn(), "Bad i32");
    if (v->num_value == 0)
      raw_type = make_unique<RawScalarType>(RawTypeValue::NIL);
    else
      raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    return error;
  }

  std::string visitCharacter(Character *) override {
    temporary = true;
    raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
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
    if (!error.empty())
      return error;
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
      std::string name = sub.substr(7, sub.size()) + "." +
                         v->member_name->getVariableName()->name;
      if (declarations.find(name) != declarations.end()) {
        raw_type = declarations[name];
        return error;
      }
      break;
    }
    case PostFixOpValue::DOT:
      if (raw_type->getRawTypeValue() != RawTypeValue::STRUCT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't access member of " + raw_type->print());
      sub = raw_type->getRawStructType()->getName();
      std::string name = sub.substr(7, sub.size()) + "." +
                         v->member_name->getVariableName()->name;
      if (declarations.find(name) != declarations.end()) {
        raw_type = declarations[name];
        if (raw_type->isFunctionPointer())
          while (raw_type->getRawTypeValue() == RawTypeValue::POINTER)
            raw_type = raw_type->deref();
        return error;
      }
      break;
    }
    return SEMANTIC_ERROR(
        v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
        "Can't find member " + v->member_name->getVariableName()->name +
            " of " + sub);
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    error = v->array_name->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    error = v->index_value->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
      if (!rhs_type->compare_equal(
              make_unique<RawScalarType>(RawTypeValue::INT)))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't index with " + rhs_type->print());
      raw_type = lhs_type->deref();
    } else if (rhs_type->compare_equal(
                   make_unique<RawScalarType>(RawTypeValue::INT))) {
      if (rhs_type->getRawTypeValue() != RawTypeValue::POINTER)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't subscript " + rhs_type->print());
      raw_type = rhs_type->deref();
    } else {
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't subscript " + lhs_type->print() + " with " +
                                rhs_type->print());
    }
    temporary = false;
    return error;
  }

  std::string visitFunctionCall(FunctionCall *v) override {
    error = v->callee_name->accept(this);
    if (!error.empty())
      return error;
    if (raw_type->getRawTypeValue() != RawTypeValue::FUNCTION)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't call " + raw_type->print());

    auto return_type = raw_type;
    auto calle_arg_types = raw_type->get_param();

    if (calle_arg_types.size() < v->callee_args.size())
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Too many arguments for " + return_type->print());

    for (unsigned int i = 0; i < calle_arg_types.size(); i++) {
      if (i >= v->callee_args.size())
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Too few arguments for " + return_type->print());
      error = v->callee_args[i]->accept(this);
      if (!error.empty())
        return error;
      if (calle_arg_types[i]->getRawTypeValue() != RawTypeValue::VOID &&
          !calle_arg_types[i]->compare_equal(raw_type))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't call " + calle_arg_types[i]->print() +
                                  " with " + raw_type->print());
    }
    raw_type = return_type->get_return();
    v->setUType(raw_type);
    temporary = true;
    return error;
  }

  std::string visitUnary(Unary *v) override {
    temporary = true;
    //    if (v->op_kind == UnaryOpValue::MINUS &&
    //        v->operand->getNumber()) {
    //      if (v->operand->getNumber()->num_value < 0 ||
    //          v->operand->getNumber()->num_value - 1 >
    //              std::numeric_limits<int>::max())
    //        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
    //                              v->getTokenRef().getColumn(), "Bad i32");
    //      else
    //        raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    //    } else {
    error = v->operand->accept(this);
    if (!error.empty())
      return error;
    //    }
    switch (v->op_kind) {
    case UnaryOpValue::MINUS:
      if (!raw_type->compare_equal(
              make_unique<RawScalarType>(RawTypeValue::INT)))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't minus " + raw_type->print());
      temporary = true;
      break;
    case UnaryOpValue::NOT:
      if (!raw_type->compare_equal(
              make_unique<RawScalarType>(RawTypeValue::INT)))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't negate " + raw_type->print());
      temporary = true;
      break;
    case UnaryOpValue::DEREFERENCE:
      if (v->getNumber() && v->getNumber()->num_value == 0) {
        raw_type = std::make_shared<RawPointerType>(
            std::make_shared<RawScalarType>(RawTypeValue::VOID));
        break;
      }
      if (raw_type->getRawTypeValue() == RawTypeValue::FUNCTION) {
        temporary = false;
        break;
      }
      if (raw_type->getRawTypeValue() != RawTypeValue::POINTER)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't dereference " + raw_type->print());
      raw_type = raw_type->deref();
      temporary = false;
      break;
    case UnaryOpValue::ADDRESS_OF:
      if (v->getNumber() && v->getNumber()->num_value == 0) {
        raw_type = std::make_shared<RawScalarType>(RawTypeValue::VOID);
      }
      raw_type = make_unique<RawPointerType>(raw_type);
      if (temporary)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't get address of temporay object");
      temporary = true;
      break;
    }
    return error;
  }

  std::string visitSizeOf(SizeOf *v) override {
    if (v->operand)
      error = v->operand->accept(this);
    else if (v->type_name)
      error = v->type_name->accept(this);
    temporary = true;
    if (raw_type->getRawTypeValue() == RawTypeValue::FUNCTION)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't get size of " + raw_type->print());
    v->setUType(raw_type);
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
    if (v->op_kind == BinaryOpValue ::MULTIPLY &&
        ((lhs_type->getRawTypeValue() != RawTypeValue::INT &&
          lhs_type->getRawTypeValue() != RawTypeValue::CHAR &&
          lhs_type->getRawTypeValue() != RawTypeValue::NIL) ||
         (rhs_type->getRawTypeValue() != RawTypeValue::INT &&
          rhs_type->getRawTypeValue() != RawTypeValue::CHAR &&
          rhs_type->getRawTypeValue() != RawTypeValue::NIL)))
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " * " + rhs_type->print());
    if (lhs_type->getRawTypeValue() == RawTypeValue::VOID)
      return SEMANTIC_ERROR(v->left_operand->getTokenRef().getLine(),
                            v->left_operand->getTokenRef().getColumn(),
                            "handling " + lhs_type->print());
    if (rhs_type->getRawTypeValue() == RawTypeValue::VOID)
      return SEMANTIC_ERROR(v->right_operand->getTokenRef().getLine(),
                            v->right_operand->getTokenRef().getColumn(),
                            "handling " + rhs_type->print());
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " and " + rhs_type->print());
    }
    if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER &&
        rhs_type->getRawTypeValue() == RawTypeValue::POINTER &&
        !lhs_type->compare_exact(rhs_type))
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " and " + rhs_type->print());
    temporary = true;
    if ((v->op_kind == BinaryOpValue::MULTIPLY ||
         v->op_kind == BinaryOpValue::ADD ||
         v->op_kind == BinaryOpValue::SUBTRACT) &&
        lhs_type->getRawTypeValue() == RawTypeValue::CHAR &&
        rhs_type->getRawTypeValue() == RawTypeValue::CHAR)
      raw_type = std::make_shared<RawScalarType>(RawTypeValue::CHAR);
    else
      raw_type = std::make_shared<RawScalarType>(RawTypeValue::INT);
    if (v->op_kind == BinaryOpValue::ADD ||
        v->op_kind == BinaryOpValue::SUBTRACT) {
      if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER &&
          rhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
        if (v->op_kind == BinaryOpValue::ADD)
          return SEMANTIC_ERROR(
              v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
              "Can't handle " + lhs_type->print() + " + " + rhs_type->print());
        raw_type = std::make_shared<RawScalarType>(RawTypeValue::INT);
      } else if (rhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
        raw_type = rhs_type;
        temporary = false;
      } else if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
        raw_type = lhs_type;
        temporary = false;
      }
    }
    return error;
  }

  std::string visitTernary(Ternary *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    if (!raw_type->compare_equal(
            make_unique<RawScalarType>(RawTypeValue::INT))) {
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
    v->setUType(raw_type);
    if (temporary || !v->left_operand->isLValue() ||
        lhs_type->getRawTypeValue() == RawTypeValue::FUNCTION)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't assign to " + lhs_type->print());
    error = v->right_operand->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    //    if (lhs_type->isVoidPtr() &&
    //        (rhs_type->getRawTypeValue() == RawTypeValue::POINTER ||
    //         rhs_type->getRawTypeValue() == RawTypeValue::FUNCTION)) {
    //      std::string name = v->left_operand->getVariableName()->name;
    //      std::string tmp_pre = prefix();
    //      std::string tmp;
    //      while (tmp_pre.find('.') != std::string::npos) {
    //        tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
    //        tmp = tmp_pre;
    //        tmp += "." + name;
    //        if (declarations.find(tmp) != declarations.end()) {
    //          declarations[tmp] = rhs_type;
    //          return error;
    //        }
    //      }
    //    }
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
