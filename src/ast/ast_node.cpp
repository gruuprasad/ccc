#include "ast_node.hpp"

#include <sstream>

namespace ccc {

std::string BinaryExpression::prettyPrint(int) {
  return "(" + this->leftExpr->prettyPrint(0) + " " + this->op + " " +
         this->rightExpr->prettyPrint(0) + ")";
}

std::string BreakStatement::prettyPrint(int lvl) {
  return indent(lvl) + "break;\n";
}

std::string CallExpression::prettyPrint(int) {
  std::stringstream ss;
  ss << "(" << call->prettyPrint(0) << "(";
  for (Expression *arg : args) {
    ss << arg->prettyPrint(0);
    if (arg != args.back())
      ss << ", ";
  }
  ss << "))";
  return ss.str();
}

std::string CompoundStatement::prettyPrintBlock(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block)
    ss << stat->prettyPrint(lvl + 1);
  return "{\n" + ss.str() + indent(lvl) + "}";
}

std::string CompoundStatement::prettyPrint(int lvl) {
  return indent(lvl) + prettyPrintBlock(lvl) + "\n";
}

std::string CompoundStatement::prettyPrintInline(int lvl) {
  return " " + prettyPrintBlock(lvl - 1) + "\n";
}

std::string CompoundStatement::prettyPrintScopeIndent(int lvl) {
  return " " + prettyPrintBlock(lvl - 1) + " ";
}

std::string CompoundStatement::prettyPrintStruct(int lvl) {
  return prettyPrintBlock(lvl) + " ";
}

std::string ContinueStatement::prettyPrint(int lvl) {
  return indent(lvl) + "continue;\n";
}

std::string DeclarationStatement::prettyPrint(int lvl) {
  return indent(lvl) + type->prettyPrint(0) +
         (body ? "\n" + body->prettyPrint(lvl) : ";\n");
}

std::string ExpressionStatement::prettyPrint(int lvl) {
  return indent(lvl) + (this->expr ? expr->prettyPrint(0) : "") + ";\n";
}

std::string GotoStatement::prettyPrint(int lvl) {
  return indent(lvl) + "goto " + this->expr->prettyPrint(0) + ";\n";
}

std::string IfElseStatement::prettyPrint(int lvl) {
  return indent(lvl) + "if (" + this->expr->prettyPrint(0) + ")" +
         (this->elseStat
              ? this->ifStat->prettyPrintScopeIndent(lvl + 1) + "else" +
                    this->elseStat->prettyPrintInlineIf(lvl + 1)
              : this->ifStat->prettyPrintInline(lvl + 1));
}

std::string IfElseStatement::prettyPrintInline(int lvl) {
  return "\n" + this->prettyPrint(lvl);
}

std::string IfElseStatement::prettyPrintInlineIf(int lvl) {
  return " if (" + this->expr->prettyPrint(0) + ")" +
         (this->elseStat ? this->ifStat->prettyPrintScopeIndent(lvl) + "else" +
                               this->elseStat->prettyPrintInlineIf(lvl)
                         : this->ifStat->prettyPrintInline(lvl));
}

std::string LabeledStatement::prettyPrint(int lvl) {
  return this->expr->prettyPrint(0) + ":\n" +
         (this->stat ? this->stat->prettyPrint(lvl) : "");
}

std::string ReturnStatement::prettyPrint(int lvl) {
  return expr ? indent(lvl) + "return " + expr->prettyPrint(0) + ";\n"
              : indent(lvl) + "return;\n";
}

std::string StructStatement::prettyPrint(int lvl) {
  return indent(lvl) + "struct " + this->name->prettyPrint(0) + "\n" +
         indent(lvl) + body->prettyPrintStruct(lvl) +
         this->alias->prettyPrint(0) + ";\n";
}

std::string TranslationUnit::prettyPrint(int lvl) {
  std::stringstream ss;
  for (ASTNode *node : this->children) {
    ss << node->prettyPrint(lvl);
    if (node != children.back())
      ss << "\n";
  }
  return ss.str();
}

std::string TypeExpression::prettyPrint(int) {
  std::string tail =
      (expr ? (expr->isTypeExpression() ? " (" + expr->prettyPrint(0) + ")"
                                        : " " + expr->prettyPrint(0))
            : "");
  switch (baseType) {
  case TypeSpec::VOID:
    return "void" + tail;
  case TypeSpec::CHAR:
    return "char" + tail;
  case TypeSpec::INT:
    return "int" + tail;
  default:
    return "?";
  }
}

std::string PointerTypeExpression::prettyPrint(int) {
  return "*" + (expr->isTypeExpression() ? "(" + expr->prettyPrint(0) + ")"
                                         : expr->prettyPrint(0));
}

std::string StructTypeExpression::prettyPrint(int) {
  return "struct " + iden->prettyPrint(0) +
         (expr ? " (" + expr->prettyPrint(0) + ")" : "");
}

std::string FunctionTypeExpression::prettyPrint(int) {
  std::stringstream ss;
  for (TypeExpression *arg : args) {
    ss << arg->prettyPrint(0);
    if (arg != args.back())
      ss << ", ";
  }
  return (expr->isTypeExpression() ? "(" + expr->prettyPrint(0) + ")("
                                   : expr->prettyPrint(0) + "(") +
         ss.str() + ")";
}

std::string WhileStatement::prettyPrint(int lvl) {
  return indent(lvl) + "while (" + this->expr->prettyPrint(0) + ")" +
         this->stat->prettyPrintInline(lvl + 1);
}

} // namespace ccc
