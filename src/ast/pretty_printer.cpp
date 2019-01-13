#include "ast_node.hpp"

namespace ccc {

std::string PrimaryExpression::prettyPrint(int) { return extra; }

std::string UnaryExpression::prettyPrint(int) {
  return "(" + op + expr->prettyPrint(0) + ")";
}

std::string BinaryExpression::prettyPrint(int) {
  return "(" + this->leftExpr->prettyPrint(0) + " " + this->op + " " +
         this->rightExpr->prettyPrint(0) + ")";
}

std::string BreakStatement::prettyPrint(int lvl) {
  return indent(lvl) + "break;\n";
}

// std::string CallExpression::prettyPrint(int) {
//  std::stringstream ss;
//  ss << "(" << call->prettyPrint(0) << "(";
//  for (const auto &arg : args) {
//    ss << arg->prettyPrint(0);
//    if (arg != args.back())
//      ss << ", ";
//  }
//  ss << "))";
//  return ss.str();
//}

std::string PostfixExpression::prettyPrint(int) {
  return "(" + expr->prettyPrint(0) + op + post->prettyPrint(0) + ")";
}

std::string ConditionalExpression::prettyPrint(int) {
  std::stringstream ss;
  ss << "(" << condExpr->prettyPrint(0) << " ? " << ifExpr->prettyPrint(0)
     << " : " << elseExpr->prettyPrint(0) << ")";
  return ss.str();
}

std::string SizeOfExpression::prettyPrint(int) {
  std::stringstream ss;
  if (expr->isTypeDeclaration())
    ss << "(sizeof(" << expr->prettyPrint(0) << "))";
  else
    ss << "(sizeof " << expr->prettyPrint(0) << ")";
  return ss.str();
}

std::string CompoundStatement::prettyPrintBlock(int lvl) {
  std::stringstream ss;
  for (const auto &stat : this->block)
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
  return indent(lvl) + "goto " + this->label->prettyPrint(0) + ";\n";
}

std::string IfElseStatement::prettyPrint(int lvl) {
  return indent(lvl) + "if (" + this->cond->prettyPrint(0) + ")" +
         (this->elseStat
              ? this->ifStat->prettyPrintScopeIndent(lvl + 1) + "else" +
                    this->elseStat->prettyPrintInlineIf(lvl + 1)
              : this->ifStat->prettyPrintInline(lvl + 1));
}

std::string IfElseStatement::prettyPrintInline(int lvl) {
  return "\n" + this->prettyPrint(lvl);
}

std::string IfElseStatement::prettyPrintInlineIf(int lvl) {
  return " if (" + this->cond->prettyPrint(0) + ")" +
         (this->elseStat ? this->ifStat->prettyPrintScopeIndent(lvl) + "else" +
                               this->elseStat->prettyPrintInlineIf(lvl)
                         : this->ifStat->prettyPrintInline(lvl));
}

std::string LabeledStatement::prettyPrint(int lvl) {
  return this->label->prettyPrint(0) + ":\n" +
         (this->stat ? this->stat->prettyPrint(lvl) : "");
}

std::string ReturnStatement::prettyPrint(int lvl) {
  return expr ? indent(lvl) + "return " + expr->prettyPrint(0) + ";\n"
              : indent(lvl) + "return;\n";
}

std::string StructStatement::prettyPrint(int lvl) {
  return indent(lvl) + "struct " + this->type->prettyPrint(0) + "\n" +
         indent(lvl) + body->prettyPrintStruct(lvl) +
         this->alias->prettyPrint(0) + ";\n";
}

std::string TranslationUnit::prettyPrint(int lvl) {
  std::stringstream ss;
  for (const auto &node : this->children) {
    ss << node->prettyPrint(lvl);
    if (node != children.back())
      ss << "\n";
  }
  return ss.str();
}

std::string TypeDeclaration::prettyPrint(int) {
  std::string tail =
      (expr ? (expr->isTypeDeclaration() ? " (" + expr->prettyPrint(0) + ")"
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

std::string PointerTypeDeclaration::prettyPrint(int) {
  return "*" + (expr->isTypeDeclaration() ? "(" + expr->prettyPrint(0) + ")"
                                          : expr->prettyPrint(0));
}

std::string StructTypeDeclaration::prettyPrint(int) {
  return "struct " + iden->prettyPrint(0) +
         (expr ? " (" + expr->prettyPrint(0) + ")" : "");
}

std::string FunctionTypeDeclaration::prettyPrint(int) {
  std::stringstream ss;
  for (const auto &arg : args) {
    ss << arg->prettyPrint(0);
    if (arg != args.back())
      ss << ", ";
  }
  return (expr->isTypeDeclaration() ? "(" + expr->prettyPrint(0) + ")("
                                    : expr->prettyPrint(0) + "(") +
         ss.str() + ")";
}

std::string WhileStatement::prettyPrint(int lvl) {
  return indent(lvl) + "while (" + this->cond->prettyPrint(0) + ")" +
         this->stat->prettyPrintInline(lvl + 1);
}

std::string Statement::indent(int n) {
  if (n >= 0) {
    std::stringstream ss;
    for (int i = 0; i < n; i++)
      ss << "\t";
    return ss.str();
  } else
    return "";
}
} // namespace ccc
