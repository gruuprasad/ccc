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

/**
 * AST visitor class for semantical analysis
 */
class SemanticVisitor : public Visitor<std::string> {
  // save all occuring identifiers with type informations
  IdentifierSetType definitions;
  IdentifierMapType declarations;
  IdentifierSetType labels;
  IdentifierPtrListType uLabels;
  // global error string
  std::string error;
  // detect nested loops
  int loop_counter;
  // passes between visits
  bool temporary = true;
  std::string alias;
  bool function_definition = false;
  // set by walking the tree bottom up, used to pass information to parent nodes
  std::shared_ptr<RawType> raw_type = nullptr;
  // return type of current function body
  std::shared_ptr<RawType> jump_type = nullptr;
  // prefixcode for identifiers
  std::vector<std::string> pre;

  // generate prefix
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

  /**
   * method to print current scopes at any point in analysis
   */
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

  /**
   * called externaly to check for errors - adressing undefined labels
   *
   * @return bool
   */
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

  /**
   * root of AST
   *
   * @param v visitor
   * @return string
   */
  std::string visitTranslationUnit(TranslationUnit *v) override {
    raw_type = nullptr;
    for (const auto &child : v->extern_list) {
      error = child->accept(this);
      if (!error.empty())
        break;
    }
    return error;
  }

  /**
   * analyse external declared method, only appears in global scope
   *
   * @param v visitor
   * @return string
   */
  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    function_definition = true;
    v->return_type->accept(this);
    // is not abstract
    if (v->fn_name && v->fn_name->getIdentifier()) {
      const auto &identifier = *v->fn_name->getIdentifier();
      auto name = prefix(identifier->name);
      pre.emplace_back(identifier->name);
      error = v->fn_name->accept(this);
      if (!error.empty())
        return error;
      // check for duplicates
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
        // set to global map
        declarations[name] = raw_type;
      if (raw_type->isFunctionPointer())
        return SEMANTIC_ERROR(v->fn_name->getTokenRef().getLine(),
                              v->fn_name->getTokenRef().getColumn(),
                              "Can't define " + raw_type->print());
      // set variables used in code gernation
      v->setUType(raw_type);
      v->setUIdentifier(name);
      // set return type of function body
      jump_type = raw_type->get_return();
    } else if (v->fn_name)
      return SEMANTIC_ERROR(v->fn_name->getTokenRef().getLine(),
                            v->fn_name->getTokenRef().getColumn(),
                            "Missing identifier");
    function_definition = false;
    error = v->fn_body->accept(this);
    // delete everything in body scope
    for (auto it = declarations.begin(); it != declarations.end();)
      if ((*it).first.compare(0, prefix("$").size(), prefix("$")) == 0)
        declarations.erase(it++);
      else
        ++it;
    pre.pop_back();
    return error;
  }

  /**
   *  predeclaration of function, basicly as above
   *
   * @param v visitor
   * @return string
   */
  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    function_definition = false;
    v->return_type->accept(this);
    if (v->fn_name && v->fn_name->getIdentifier()) {
      const auto &identifier = *v->fn_name->getIdentifier();
      auto name = prefix(identifier->name);
      v->fn_name->accept(this);
      if (declarations.find(name) != declarations.end()) {
        // allow redefinition with same type
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

  /**
   * check declaration of identifier with type
   *
   * @param v visitor
   * @return string
   */
  std::string visitDataDeclaration(DataDeclaration *v) override {
    error = v->data_type->accept(this);
    if (!error.empty())
      return error;
    if (v->data_name && v->data_name->getIdentifier()) {
      const auto &identifier = *v->data_name->getIdentifier();
      std::string name = prefix(identifier->name);
      v->data_name->accept(this);
      if (declarations.find(name) != declarations.end()) {
        // not global
        if (!(prefix() == "$."))
          // lookup redefinition
          return SEMANTIC_ERROR(identifier->getTokenRef().getLine(),
                                identifier->getTokenRef().getColumn(),
                                "Redefinition of '" + identifier->name + "'");
        // allow gloabl redefinition with same type
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
      // set variables used in code gernation
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

  /**
   *  check struct and keep scoping (nested structure) information
   *
   * @param v visitor
   * @return string
   */
  std::string visitStructDeclaration(StructDeclaration *v) override {
    error = v->struct_type->accept(this);
    // type wasn't set yet
    bool anonymous = !raw_type;
    if (!error.empty())
      return error;
    if (v->struct_alias) {
      const auto &identifier = *v->struct_alias->getIdentifier();
      std::string name = prefix(identifier->name);
      if (anonymous)
        raw_type = std::make_shared<RawStructType>(
            "struct " + prefix("__" + prefix(identifier->name) + "__"));
      auto tmp = raw_type;
      // nameless struct, use alias as scoping information
      if (anonymous && (*v->struct_type->getStructType()).is_definition) {
        pre.emplace_back("__" + name + "__");
        for (const auto &d : (*v->struct_type->getStructType()).member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
          if (!d->getUType()->elem_size.empty())
            tmp->elem_size.insert(tmp->elem_size.end(),
                                  d->getUType()->elem_size.begin(),
                                  d->getUType()->elem_size.end());
          else
            tmp->elem_size.push_back(d->getUType()->size());
        }
        pre.pop_back();
      }
      raw_type = tmp;
      v->struct_alias->accept(this);
      if (declarations.find(name) != declarations.end()) {
        // not global or anonymous
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
    } else if (anonymous) {
      // basic support for anonymous structs, flatmaps to current scope
      for (const auto &d : (*v->struct_type->getStructType()).member_list) {
        error = d->accept(this);
        if (!error.empty())
          return error;
      }
    }
    // set variables used in code gernation
    v->setUType(raw_type);
    return error;
  }

  /**
   * only appears in parameter list, works as declaration to body scope
   *
   * @param v visitor
   * @return string
   */
  std::string visitParamDeclaration(ParamDeclaration *v) override {
    if (function_definition && !v->param_name)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Expected identifier");
    // allow nested function types without parameter names
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
      v->setUType(raw_type);
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

  /**
   * container for scalar types
   *
   * @param v visitor
   * @return string
   */
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
    v->setUType(raw_type);
    return error;
  }

  /**
   * container for struct types
   *
   * @param v visitor
   * @return string
   */
  std::string visitStructType(StructType *v) override {
    // not nameless
    if (v->struct_name) {
      raw_type = nullptr;
      if (v->is_definition) {
        // allow redefinitions in different scopes (gcc doesn't)
      } else {
        std::string tmp;
        std::string tmp_pre = prefix();
        while (tmp_pre.find('.') != std::string::npos) {
          tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
          tmp = tmp_pre + ".__" + v->struct_name->name + "__";
          if (declarations.find(tmp) != declarations.end()) {
            raw_type = declarations.find(tmp)->second;
            v->elem_size = raw_type->elem_size;
            break;
          }
        }
      }
      // calculate unique name
      auto name = prefix("__" + v->struct_name->name + "__");
      if (!raw_type) {
        raw_type = make_unique<RawStructType>(
            "struct " + prefix("__" + v->struct_name->name + "__"));
        declarations[name] = raw_type;
      }
      auto ret = raw_type;
      if (v->is_definition) {
        if (definitions.find(name) != definitions.end())
          return SEMANTIC_ERROR(v->struct_name->getTokenRef().getLine(),
                                v->struct_name->getTokenRef().getColumn(),
                                "Redefinition of 'struct " +
                                    v->struct_name->name + "'");
        pre.push_back("__" + v->struct_name->name + "__");
        // used for saving sizeof members
        v->elem_size.clear();
        // define members of struct
        for (const auto &d : v->member_list) {
          error = d->accept(this);
          if (!error.empty())
            return error;
          if (!d->getUType()->elem_size.empty())
            v->elem_size.insert(v->elem_size.end(),
                                d->getUType()->elem_size.begin(),
                                d->getUType()->elem_size.end());
          else
            v->elem_size.push_back(d->getUType()->size());
        }
        definitions.insert(name);
        pre.pop_back();
      }
      raw_type = ret;
      raw_type->elem_size = v->elem_size;
      v->setUType(raw_type);
    } else {
      // nameless, return null to struct declaration
      raw_type = nullptr;
    }
    return error;
  }

  /**
   * container for abstract types
   *
   * @param v visitor
   * @return string
   */
  std::string visitAbstractType(AbstractType *v) override {
    v->type->accept(this);
    for (int i = 0; i < v->ptr_count; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
    v->setUType(raw_type);
    return error;
  }

  /**
   * break recursive raw type generation
   *
   * @return string
   */
  std::string visitDirectDeclarator(DirectDeclarator *) override {
    // EMPTY
    return error;
  }

  /**
   * container for abstract declarators
   *
   * @param v visitor
   * @return string
   */
  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    for (unsigned int i = 0; i < v->pointerCount; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
    v->setUType(raw_type);
    return error;
  }

  /**
   * container for pointer declarators
   *
   * @param v visitor
   * @return string
   */
  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    v->identifier->accept(this);
    for (int i = 0; i < v->indirection_level; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
    v->setUType(raw_type);
    return error;
  }

  /**
   * container for method declarators
   *
   * @param v visitor
   * @return string
   */
  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    // open a new scope for parameter list which will be kept for visiting body
    pre.emplace_back("$");
    v->identifier->accept(this);
    if (!error.empty())
      return error;
    // wrapping function pointer - fixing an issue resulting from parsing
    // abstract return type instead of function pointer
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
    // build function pointer
    for (int i = 0; i < lvl; i++)
      raw_type = make_unique<RawPointerType>(raw_type);
    return error;
  }

  /**
   * handle nested statements
   *
   * @param v visitor
   * @return string
   */
  std::string visitCompoundStmt(CompoundStmt *v) override {
    // open a new scope by pushing prefix
    pre.emplace_back("$");
    // visit all children
    for (const auto &stat : v->block_items) {
      error = stat->accept(this);
      if (!error.empty())
        break;
    }
    // delete all nested declarations and definitions
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
    // pop prefix
    pre.pop_back();
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitIfElse(IfElse *v) override {
    error = v->condition->accept(this);
    if (!error.empty())
      return error;
    // insist on boolean value
    if (!raw_type->compare_equal(
            make_unique<RawScalarType>(RawTypeValue::INT))) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Condition has to be int, found " + raw_type->print());
    }
    // if statemnt in own scope
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
    // else can be empty
    if (v->elseStmt) {
      // else statement in own scope
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

  /**
   * @param v visitor
   * @return string
   */
  std::string visitLabel(Label *v) override {
    auto label = v->label_name->name;
    for (const auto &l : labels) {
      if (l == label) {
        return SEMANTIC_ERROR(v->label_name->getTokenRef().getLine(),
                              v->label_name->getTokenRef().getColumn(),
                              "Redefinition of label '" + label + "'");
      }
    }
    // keep label as defined
    labels.insert(label);
    return v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    error = v->predicate->accept(this);
    if (!error.empty())
      return error;
    // insist on boolean value
    if (!raw_type->compare_equal(
            make_unique<RawScalarType>(RawTypeValue::INT))) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Predicate has to be int, found " + raw_type->print());
    }
    // keep track of nested loops
    loop_counter++;
    // insert own scope for lop body
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

  /**
   * check jump instruction
   *
   * @param v visitor
   * @return string
   */
  std::string visitGoto(Goto *v) override {
    auto label = v->label_name->name;
    // lookup label definitions
    for (const auto &l : labels)
      if (l == label)
        return error;
    // label wasn't defined yet, so keep it in mind for later
    uLabels.push_back(&v->label_name);
    return error;
  }

  /**
   * wrapper for expressions as statements
   *
   * @param v visitor
   * @return string
   */
  std::string visitExpressionStmt(ExpressionStmt *v) override {
    if (v->expr)
      return v->expr->accept(this);
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitBreak(Break *v) override {
    // not in a loop
    if (loop_counter <= 0)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "'break' statement not in a loop statement");
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitReturn(Return *v) override {
    // returns a value, which has to be of current return type
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

  /**
   * @param v visitor
   * @return string
   */
  std::string visitContinue(Continue *v) override {
    // not in a loop
    if (loop_counter <= 0)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "'continue' statement not in a loop statement");
    return error;
  }

  /**
   * visit identifier, only called inside of expressions
   *
   * @param v visitor
   * @return string
   */
  std::string visitVariableName(VariableName *v) override {
    temporary = false;
    // find identifier in declarations / outer scopes
    std::string name;
    std::string tmp_pre = prefix();
    while (tmp_pre.find('.') != std::string::npos) {
      tmp_pre = tmp_pre.substr(0, tmp_pre.find_last_of('.'));
      name = tmp_pre + "." + v->name;
      if (declarations.find(name) != declarations.end()) {
        raw_type = declarations[name];
        v->setUIdentifier(name);
        v->setUType(raw_type);
        return error;
      }
    }
    return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                          v->getTokenRef().getColumn(),
                          "Use of undeclared identifier '" + name + "'");
  }

  /**
   * constant
   *
   * @param v visitor
   * @return string
   */
  std::string visitNumber(Number *v) override {
    temporary = v->num_value != 0;
    // use nil type to represent either nullptr or actual number 0
    if (v->num_value == 0)
      raw_type = make_unique<RawScalarType>(RawTypeValue::NIL);
    else
      raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    v->setUType(raw_type);
    return error;
  }

  /**
   * constant
   *
   * @param v visitor
   * @return string
   */
  std::string visitCharacter(Character *v) override {
    temporary = true;
    raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    v->setUType(raw_type);
    return error;
  }

  /**
   * string literal
   *
   * @param v visitor
   * @return string
   */
  std::string visitString(String *v) override {
    temporary = false;
    raw_type = make_unique<RawPointerType>(
        make_unique<RawScalarType>(RawTypeValue::CHAR));
    v->setUType(raw_type);
    return error;
  }

  /**
   * accesing members of a struct
   *
   * @param v visitor
   * @return string
   */
  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    error = v->struct_name->accept(this);
    if (!error.empty())
      return error;
    temporary = false;
    std::string sub;
    switch (v->op_kind) {
    case PostFixOpValue::ARROW: {
      // callee has to be pointer to struct
      if (raw_type->getRawTypeValue() != RawTypeValue::POINTER)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't dereference " + raw_type->print());
      if (raw_type->deref()->getRawTypeValue() != RawTypeValue::STRUCT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't access member of " + raw_type->print());
      // find member
      sub = raw_type->deref()->getRawStructType()->getName();
      std::string name = sub.substr(7, sub.size()) + "." +
                         v->member_name->getVariableName()->name;
      if (declarations.find(name) != declarations.end()) {
        raw_type = declarations[name];
        // enable function pointer access without dereferencing
        if (raw_type->isFunctionPointer())
          while (raw_type->getRawTypeValue() == RawTypeValue::POINTER)
            raw_type = raw_type->deref();
        v->setUType(raw_type);
        return error;
      }
      break;
    }
    case PostFixOpValue::DOT:
      // callee has to be struct
      if (raw_type->getRawTypeValue() != RawTypeValue::STRUCT)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't access member of " + raw_type->print());
      // find member
      sub = raw_type->getRawStructType()->getName();
      std::string name = sub.substr(7, sub.size()) + "." +
                         v->member_name->getVariableName()->name;
      if (declarations.find(name) != declarations.end()) {
        raw_type = declarations[name];
        // enable function pointer access without dereferencing
        if (raw_type->isFunctionPointer())
          while (raw_type->getRawTypeValue() == RawTypeValue::POINTER)
            raw_type = raw_type->deref();
        v->setUType(raw_type);
        return error;
      }
      break;
    }
    return SEMANTIC_ERROR(
        v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
        "Can't find member " + v->member_name->getVariableName()->name +
            " of " + sub);
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    error = v->array_name->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    error = v->index_value->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    // either lhs or rhs have to be pointer and index value
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
    v->setUType(raw_type);
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitFunctionCall(FunctionCall *v) override {
    error = v->callee_name->accept(this);
    if (!error.empty())
      return error;
    if (raw_type->isFunctionPointer())
      raw_type = raw_type->deref();
    // callee has to be function
    if (raw_type->getRawTypeValue() != RawTypeValue::FUNCTION)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't call " + raw_type->print());
    auto return_type = raw_type;
    auto calle_arg_types = raw_type->get_param();
    // wrong number of arguments
    if (calle_arg_types.size() < v->callee_args.size())
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Too many arguments for " + return_type->print());
    // compare passed values to expected arguments
    for (unsigned int i = 0; i < calle_arg_types.size(); i++) {
      if (i >= v->callee_args.size())
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Too few arguments for " + return_type->print());
      error = v->callee_args[i]->accept(this);
      if (!error.empty())
        return error;
      v->callee_args[i]->setUType(calle_arg_types[i]);
      if (calle_arg_types[i]->getRawTypeValue() != RawTypeValue::VOID &&
          !calle_arg_types[i]->compare_equal(raw_type))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't call " + calle_arg_types[i]->print() +
                                  " with " + raw_type->print());
    }
    // pass back return type
    raw_type = return_type->get_return();
    v->setUType(raw_type);
    temporary = true;
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitUnary(Unary *v) override {
    temporary = true;
    error = v->operand->accept(this);
    if (!error.empty())
      return error;
    //    }
    switch (v->op_kind) {
    case UnaryOpValue::MINUS:
      // only applicable on numbers
      if (!raw_type->compare_equal(
              make_unique<RawScalarType>(RawTypeValue::INT)))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't minus " + raw_type->print());
      temporary = true;
      break;
    case UnaryOpValue::NOT:
      // only applicable on boolean values
      if (!raw_type->compare_equal(
              make_unique<RawScalarType>(RawTypeValue::INT)))
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't negate " + raw_type->print());
      temporary = true;
      break;
    case UnaryOpValue::DEREFERENCE:
      // represent nullptr as void*
      if (v->getNumber() && v->getNumber()->num_value == 0) {
        raw_type = std::make_shared<RawPointerType>(
            std::make_shared<RawScalarType>(RawTypeValue::VOID));
        break;
      }
      // dereference only pointer, value is not temporary
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
      // represent &0 as void
      if (v->getNumber() && v->getNumber()->num_value == 0) {
        raw_type = std::make_shared<RawScalarType>(RawTypeValue::VOID);
      }
      // create pointer to value, which is temporary
      raw_type = make_unique<RawPointerType>(raw_type);
      if (temporary)
        return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                              v->getTokenRef().getColumn(),
                              "Can't get address of temporay object");
      temporary = true;
      break;
    }
    v->setUType(raw_type);
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitSizeOf(SizeOf *v) override {
    // either expression or type
    if (v->operand) {
      error = v->operand->accept(this);
    } else if (v->type_name) {
      error = v->type_name->accept(this);
    }
    if (!error.empty())
      return error;
    temporary = true;
    // return integer value
    raw_type = make_unique<RawScalarType>(RawTypeValue::INT);
    v->setUType(raw_type);
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitBinary(Binary *v) override {
    error = v->left_operand->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    error = v->right_operand->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    // enforce restrictions on multiplication
    if (v->op_kind == BinaryOpValue::MULTIPLY &&
        ((lhs_type->getRawTypeValue() != RawTypeValue::INT &&
          lhs_type->getRawTypeValue() != RawTypeValue::CHAR &&
          lhs_type->getRawTypeValue() != RawTypeValue::NIL) ||
         (rhs_type->getRawTypeValue() != RawTypeValue::INT &&
          rhs_type->getRawTypeValue() != RawTypeValue::CHAR &&
          rhs_type->getRawTypeValue() != RawTypeValue::NIL)))
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " * " + rhs_type->print());
    // can't handle void
    if (lhs_type->getRawTypeValue() == RawTypeValue::VOID)
      return SEMANTIC_ERROR(v->left_operand->getTokenRef().getLine(),
                            v->left_operand->getTokenRef().getColumn(),
                            "handling " + lhs_type->print());
    if (rhs_type->getRawTypeValue() == RawTypeValue::VOID)
      return SEMANTIC_ERROR(v->right_operand->getTokenRef().getLine(),
                            v->right_operand->getTokenRef().getColumn(),
                            "handling " + rhs_type->print());
    // need to be able to cast both sides into each other
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " and " + rhs_type->print());
    }
    // pointer arithmetic
    if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER &&
        rhs_type->getRawTypeValue() == RawTypeValue::POINTER &&
        !lhs_type->compare_exact(rhs_type))
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't handle " + lhs_type->print() + " and " + rhs_type->print());
    temporary = true;
    // cast int and char
    if ((v->op_kind == BinaryOpValue::MULTIPLY ||
         v->op_kind == BinaryOpValue::ADD ||
         v->op_kind == BinaryOpValue::SUBTRACT) &&
        lhs_type->getRawTypeValue() == RawTypeValue::CHAR &&
        rhs_type->getRawTypeValue() == RawTypeValue::CHAR)
      raw_type = std::make_shared<RawScalarType>(RawTypeValue::CHAR);
    else
      raw_type = std::make_shared<RawScalarType>(RawTypeValue::INT);
    // pointer arithmetic 2.0, result not temporary
    if (v->op_kind == BinaryOpValue::ADD ||
        v->op_kind == BinaryOpValue::SUBTRACT) {
      if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER &&
          rhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
        if (v->op_kind == BinaryOpValue::ADD)
          return SEMANTIC_ERROR(
              v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
              "Can't handle " + lhs_type->print() + " + " + rhs_type->print());
        // ptrdiff
        raw_type = std::make_shared<RawScalarType>(RawTypeValue::INT);
        raw_type->setSize(8);
      } else if (rhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
        raw_type = rhs_type;
        temporary = false;
      } else if (lhs_type->getRawTypeValue() == RawTypeValue::POINTER) {
        raw_type = lhs_type;
        temporary = false;
      }
    }
    v->setUType(raw_type);
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
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
    // branches have to have castable type
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't branch with " + lhs_type->print() + " and " +
                                rhs_type->print());
    }
    // result is temporary
    temporary = true;
    v->setUType(raw_type);
    return error;
  }

  /**
   * @param v visitor
   * @return string
   */
  std::string visitAssignment(Assignment *v) override {
    error = v->left_operand->accept(this);
    if (!error.empty())
      return error;
    auto lhs_type = raw_type;
    // lhs has to be non temporary lvalue
    if (temporary || !v->left_operand->isLValue() ||
        lhs_type->getRawTypeValue() == RawTypeValue::FUNCTION)
      return SEMANTIC_ERROR(v->getTokenRef().getLine(),
                            v->getTokenRef().getColumn(),
                            "Can't assign to " + lhs_type->print());
    error = v->right_operand->accept(this);
    if (!error.empty())
      return error;
    auto rhs_type = raw_type;
    // rhs has to be cast into lhs
    if (!lhs_type->compare_equal(rhs_type)) {
      return SEMANTIC_ERROR(
          v->getTokenRef().getLine(), v->getTokenRef().getColumn(),
          "Can't assign " + rhs_type->print() + " to " + lhs_type->print());
    }
    // pass rhs as result, which is temporary
    temporary = true;
    v->setUType(raw_type);
    return error;
  }
};
} // namespace ccc
