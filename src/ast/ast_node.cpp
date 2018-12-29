#include "ast_node.hpp"

#include <sstream>

namespace ccc {

// Methods to generate prettyprinting for different AST types.
std::string BinaryExpression::prettyPrint() {
  return "(" + children[0]->prettyPrint() + " " + token->name() + " " +
         children[1]->prettyPrint() + ")";
}

std::string CompoundStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl + 1);
  }
  return indent(lvl) + "{\n" + ss.str() + indent(lvl) + "}\n";
}

std::string CompoundStatement::prettyPrintBlock(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl);
  }
  return " {\n" + ss.str() + indent(lvl - 1) + "}\n";
}

std::string ExpressionStatement::prettyPrint(int lvl) {
  if (this->expr)
    return indent(lvl) + expr->prettyPrint() + ";\n";
  else
    return indent(lvl) + ";\n";
}

std::string CompoundStatement::prettyPrintScopeIndent(int lvl) {
  std::stringstream ss;
  for (Statement *stat : this->block) {
    ss << stat->prettyPrint(lvl);
  }
  return " {\n" + ss.str() + indent(lvl - 1) + "} ";
}

std::string IfElseStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  if (this->elseStat) {
    ss << indent(lvl) << "if (" << this->expr->prettyPrint() << ")"
       << this->ifStat->prettyPrintScopeIndent(lvl + 1);
    ss << "else" << this->elseStat->prettyPrintInlineIf(lvl + 1);
  } else {
    ss << indent(lvl) << "if (" << this->expr->prettyPrint() << ")"
       << this->ifStat->prettyPrintBlock(lvl + 1);
  }
  return ss.str();
}

std::string IfElseStatement::prettyPrintBlock(int lvl) {
  return "\n" + this->prettyPrint(lvl);
}

std::string IfElseStatement::prettyPrintInlineIf(int lvl) {
  std::stringstream ss;
  if (this->elseStat) {
    ss << " if (" << this->expr->prettyPrint() << ")"
       << this->ifStat->prettyPrintScopeIndent(lvl);
    ss << "else" << this->elseStat->prettyPrintBlock(lvl);
  } else {
    ss << " if (" << this->expr->prettyPrint() << ")"
       << this->ifStat->prettyPrintBlock(lvl);
  }
  return ss.str();
}

std::string WhileStatement::prettyPrint(int lvl) {
  std::stringstream ss;
  ss << indent(lvl) << "while (" << this->expr->prettyPrint() << ")"
     << this->stat->prettyPrintBlock(lvl + 1);
  return ss.str();
}

std::string BreakStatement::prettyPrint(int lvl) {
  return indent(lvl) + "break;\n";
}

std::string ContinueStatement::prettyPrint(int lvl) {
  return indent(lvl) + "continue;\n";
}

std::string ReturnStatement::prettyPrint(int lvl) {
  if (expr)
    return indent(lvl) + "return " + expr->prettyPrint() + ";\n";
  else
    return indent(lvl) + "return;\n";
}

std::string TranslationUnit::prettyPrint(int lvl) {
  std::stringstream ss;
  for (ASTNode *child : this->children)
    ss << child->prettyPrint(lvl);
  return ss.str();
}

// Methods to generate dot language of graphviz for different AST types.

std::string TranslationUnit::graphWalker() {
  if (children.empty() && this->token) {
    std::stringstream ss;
    ss << (unsigned long)this << "[label=<<font point-size='10'>"
       << *this->token << "</font>";
    ss << "> shape=none style=filled fillcolor=lightgrey];\n";
    return ss.str();
  } else {
    std::stringstream ss;
    for (ASTNode *child : children) {
      ss << child->graphWalker();
    }
    return ss.str();
  }
}

std::string Declaration::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=\"" << this->name
     << "\" shape=box style=filled fillcolor=lightsalmon];\n";
  return ss.str();
}

std::string Expression::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << *this->token << "</font>";
  ;
  ss << "> shape=oval style=filled fillcolor=lightskyblue];\n";
  for (ASTNode *child : this->children) {
    if (child) {
      ss << child->graphWalker() << (unsigned long)this << "--"
         << (unsigned long)child << std::endl;
    }
  }
  return ss.str();
}

std::string PrimaryExpression::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=<" << this->token->getExtra()
     << "> shape=diamond style=filled fillcolor=lightyellow];\n";
  return ss.str();
}

std::string Statement::graphWalker() {
  std::stringstream ss;
  ss << (unsigned long)this << "[label=<" << this->name;
  if (this->token)
    ss << "<br/><font point-size='10'>" << *this->token << "</font>";
  ss << "> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *child : this->children) {
    if (child) {
      ss << "subgraph cluster_" << (unsigned long)child << "{\n"
         << child->graphWalker() << "}\n";
      ss << (unsigned long)this << "--" << (unsigned long)child << ";\n";
    }
  }
  return ss.str();
}
} // namespace ccc
