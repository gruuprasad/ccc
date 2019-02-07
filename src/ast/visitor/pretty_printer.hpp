#ifndef C4_PRETTY_PRINTER_VISITOR_HPP
#define C4_PRETTY_PRINTER_VISITOR_HPP
#include "../../utils/utils.hpp"
#include "../ast_node.hpp"

#include <sstream>
#include <string>

#define BIG_INDENT                                                             \
  switch (indent_mod) {                                                        \
  case IndentModifier::INLINE:                                                 \
    indent_mod = IndentModifier::DEFAULT;                                      \
    return "\n" + v->accept(this);                                             \
  case IndentModifier::IF:                                                     \
    indent_mod = IndentModifier::INLINE;                                       \
    return v->accept(this);                                                    \
  case IndentModifier::SCOPE:                                                  \
    indent_mod = IndentModifier::INLINE;                                       \
    return v->accept(this) + SMALL_INDENT;                                     \
  default:                                                                     \
    indent_mod = IndentModifier::DEFAULT;                                      \
    break;                                                                     \
  }

#define INDENT std::string(indent_lvl, '\t')
#define SMALL_INDENT std::string(indent_lvl - 1, '\t')

namespace ccc {

class PrettyPrinterVisitor : public Visitor<std::string> {

  std::unordered_map<BinaryOpValue, std::string, EnumClassHash>
      BinaryOpValueToString{
          {BinaryOpValue::MULTIPLY, " * "},
          {BinaryOpValue::ADD, " + "},
          {BinaryOpValue::SUBTRACT, " - "},
          {BinaryOpValue::LESS_THAN, " < "},
          {BinaryOpValue::EQUAL, " == "},
          {BinaryOpValue::NOT_EQUAL, " != "},
          {BinaryOpValue::LOGICAL_AND, " && "},
          {BinaryOpValue::LOGICAL_OR, " || "},
      };

  std::unordered_map<UnaryOpValue, std::string, EnumClassHash>
      UnaryOpValueToString{{UnaryOpValue::ADDRESS_OF, "&"},
                           {UnaryOpValue::DEREFERENCE, "*"},
                           {UnaryOpValue::MINUS, "-"},
                           {UnaryOpValue::NOT, "!"}};

  enum class IndentModifier { DEFAULT, INLINE, IF, SCOPE };

  unsigned int indent_lvl = 0;
  IndentModifier indent_mod = IndentModifier::DEFAULT;
  std::string error;

public:
  PrettyPrinterVisitor() = default;
  ~PrettyPrinterVisitor() override = default;

  std::string visitTranslationUnit(TranslationUnit *v) override {
    std::stringstream ss;
    for (const auto &p : v->extern_list) {
      ss << p->accept(this);
      if (p != v->extern_list.back())
        ss << "\n";
    }
    return ss.str();
  }

  std::string visitFunctionDefinition(FunctionDefinition *v) override {
    return INDENT + v->return_type->accept(this) + " " +
           v->fn_name->accept(this) + "\n" + v->fn_body->accept(this);
  }

  std::string visitFunctionDeclaration(FunctionDeclaration *v) override {
    if (v->fn_name)
      return INDENT + v->return_type->accept(this) + " " +
             v->fn_name->accept(this) + ";\n";
    return INDENT + v->return_type->accept(this) + ";\n";
  }

  std::string visitDataDeclaration(DataDeclaration *v) override {
    BIG_INDENT;
    return INDENT + v->data_type->accept(this) +
           (v->data_name ? " " + v->data_name->accept(this) : error) + ";\n";
  }

  std::string visitStructDeclaration(StructDeclaration *v) override {
    BIG_INDENT;
    return INDENT + v->struct_type->accept(this) +
           (v->struct_alias ? " " + v->struct_alias->accept(this) : error) +
           ";\n";
  }

  std::string visitParamDeclaration(ParamDeclaration *v) override {
    return v->param_type->accept(this) +
           (v->param_name ? " " + v->param_name->accept(this) : "");
  }

  std::string visitScalarType(ScalarType *v) override {
    switch (v->type_kind) {
    case ScalarTypeValue::VOID:
      return "void";
    case ScalarTypeValue::CHAR:
      return "char";
    case ScalarTypeValue::INT:
      return "int";
    default:
      return error;
    };
  }

  std::string visitStructType(StructType *v) override {
    std::stringstream ss;
    ss << "struct";
    if (v->struct_name)
      ss << " " << v->struct_name->accept(this);
    if (!v->is_definition) {
      return ss.str();
    }
    ss << "\n" << INDENT << "{\n";
    for (const auto &member : v->member_list) {
      indent_lvl++;
      ss << member->accept(this);
      indent_lvl--;
    }
    return ss.str() + INDENT + "}";
  }

  std::string visitAbstractType(AbstractType *v) override {
    std::string pre, post;
    for (int i = 0; i < v->ptr_count; i++) {
      pre += "(*";
      post += ")";
    }
    return v->type->accept(this) + " " + pre + post;
  }

  std::string visitDirectDeclarator(DirectDeclarator *v) override {
    return v->identifer->accept(this);
  }

  std::string visitAbstractDeclarator(AbstractDeclarator *v) override {
    if (v->type_kind == AbstractDeclType::Data) {
      std::string pre, post;
      for (unsigned int i = 0; i < v->pointerCount; i++) {
        pre += "(*";
        post += ")";
      }
      return pre + post;
    } else {
      return error;
    }
  }

  std::string visitPointerDeclarator(PointerDeclarator *v) override {
    std::string pre, post;
    for (int i = 0; i < v->indirection_level; i++) {
      pre += "(*";
      post += ")";
    }
    return pre + (v->identifier ? v->identifier->accept(this) : error) + post;
  }

  std::string visitFunctionDeclarator(FunctionDeclarator *v) override {
    std::stringstream ss;
    std::string pre, post;
    if (v->return_ptr != nullptr) {
      const auto &abstract = *v->return_ptr->getAbstractDeclarator();
      for (unsigned int i = 0; i < abstract.pointerCount; i++) {
        pre += "(*";
        post += ")";
      }
    }
    for (const auto &p : v->param_list) {
      ss << p->accept(this);
      if (p != v->param_list.back())
        ss << ", ";
    }
    return pre + "(" + v->identifier->accept(this) + "(" + ss.str() + "))" +
           post;
  }

  std::string visitCompoundStmt(CompoundStmt *v) override {
    std::stringstream ss;
    switch (indent_mod) {
    case IndentModifier::INLINE:
      indent_mod = IndentModifier::DEFAULT;
      for (const auto &stat : v->block_items)
        ss << stat->accept(this);
      return " {\n" + ss.str() + SMALL_INDENT + "}\n";
    case IndentModifier::IF:
      indent_mod = IndentModifier::INLINE;
      return v->accept(this);
    case IndentModifier::SCOPE:
      indent_mod = IndentModifier::DEFAULT;
      for (const auto &stat : v->block_items)
        ss << stat->accept(this);
      return " {\n" + ss.str() + SMALL_INDENT + "} ";
    default:
      indent_mod = IndentModifier::DEFAULT;
      break;
    }
    indent_lvl++;
    for (const auto &stat : v->block_items)
      ss << stat->accept(this);
    indent_lvl--;
    return INDENT + "{\n" + ss.str() + INDENT + "}\n";
  }

  std::string visitIfElse(IfElse *v) override {
    std::stringstream ss;
    switch (indent_mod) {
    case IndentModifier::INLINE:
      indent_mod = IndentModifier::DEFAULT;
      return "\n" + v->accept(this);
    case IndentModifier::IF:
      ss << " if (" + v->condition->accept(this) + ")";
      if (v->elseStmt) {
        indent_mod = IndentModifier::SCOPE;
        ss << v->ifStmt->accept(this);
        indent_mod = IndentModifier::IF;
        ss << "else" << v->elseStmt->accept(this);
      } else {
        indent_mod = IndentModifier::INLINE;
        ss << v->ifStmt->accept(this);
      }
      return ss.str();
    case IndentModifier::SCOPE:
      indent_mod = IndentModifier::INLINE;
      return v->accept(this) + SMALL_INDENT;
    default:
      indent_mod = IndentModifier::DEFAULT;
      break;
    }
    ss << INDENT + "if (" + v->condition->accept(this) + ")";
    indent_lvl++;
    if (v->elseStmt) {
      indent_mod = IndentModifier::SCOPE;
      ss << v->ifStmt->accept(this);
      indent_mod = IndentModifier::IF;
      ss << "else" << v->elseStmt->accept(this);
    } else {
      indent_mod = IndentModifier::INLINE;
      ss << v->ifStmt->accept(this);
    }
    indent_lvl--;
    return ss.str();
  }

  std::string visitLabel(Label *v) override {
    BIG_INDENT;
    return v->label_name->accept(this) + ":\n" + v->stmt->accept(this);
  }

  std::string visitWhile(While *v) override {
    BIG_INDENT;
    std::stringstream ss;
    indent_mod = IndentModifier::DEFAULT;
    ss << INDENT + "while (" + v->predicate->accept(this) + ")";
    indent_mod = IndentModifier::INLINE;
    indent_lvl++;
    ss << v->block->accept(this);
    indent_lvl--;
    return ss.str();
  }

  std::string visitGoto(Goto *v) override {
    BIG_INDENT;
    return INDENT + "goto " + v->label_name->accept(this) + ";\n";
  }

  std::string visitExpressionStmt(ExpressionStmt *v) override {
    BIG_INDENT;
    return INDENT + (v->expr ? v->expr->accept(this) : error) + ";\n";
  }

  std::string visitBreak(Break *v) override {
    BIG_INDENT;
    return INDENT + "break;\n";
  }

  std::string visitReturn(Return *v) override {
    BIG_INDENT;
    std::stringstream ss;
    ss << INDENT + "return" + (v->expr ? " " + v->expr->accept(this) : "") +
              ";\n";
    return ss.str();
  }

  std::string visitContinue(Continue *v) override {
    BIG_INDENT;
    return INDENT + "continue;\n";
  }

  std::string visitVariableName(VariableName *v) override { return v->name; }

  std::string visitNumber(Number *v) override {
    return std::to_string(v->num_value);
  }

  std::string visitCharacter(Character *v) override {
    return "\'" + v->char_value + "\'";
  }

  std::string visitString(String *v) override {
    return "\"" + v->str_value + "\"";
  }

  std::string visitMemberAccessOp(MemberAccessOp *v) override {
    return "(" + v->struct_name->accept(this) +
           (v->op_kind == PostFixOpValue::DOT ? "." : "->") +
           v->member_name->accept(this) + ")";
  }

  std::string visitArraySubscriptOp(ArraySubscriptOp *v) override {
    return "(" + v->array_name->accept(this) + "[" +
           v->index_value->accept(this) + "])";
  }

  std::string visitFunctionCall(FunctionCall *v) override {
    std::stringstream ss;
    for (const auto &p : v->callee_args) {
      ss << p->accept(this);
      if (p != v->callee_args.back())
        ss << ", ";
    }
    return "(" + v->callee_name->accept(this) + "(" + ss.str() + "))";
  }

  std::string visitUnary(Unary *v) override {
    return "(" + UnaryOpValueToString[v->op_kind] + v->operand->accept(this) +
           ")";
  }

  std::string visitSizeOf(SizeOf *v) override {
    return "(sizeof" +
           (v->type_name ? "(" + v->type_name->accept(this) + ")"
                         : " " + v->operand->accept(this)) +
           ")";
  }

  std::string visitBinary(Binary *v) override {
    return "(" + v->left_operand->accept(this) +
           BinaryOpValueToString[v->op_kind] + v->right_operand->accept(this) +
           ")";
  }

  std::string visitTernary(Ternary *v) override {
    return "(" + v->predicate->accept(this) + " ? " +
           v->left_branch->accept(this) + " : " +
           v->right_branch->accept(this) + ")";
  }

  std::string visitAssignment(Assignment *v) override {
    return "(" + v->left_operand->accept(this) + " = " +
           v->right_operand->accept(this) + ")";
  }
};

} // namespace ccc
#endif
