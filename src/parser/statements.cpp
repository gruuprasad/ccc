#include <utility>

#include <utility>

#include <utility>
#include <sstream>

#include "statements.hpp"
#include "ASTNode.hpp"

ExpressionStatement::ExpressionStatement(int id, ASTNode *expression) : ASTNode(id, "expression-statement") {
  this->expression = expression;
}

std::string ExpressionStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->expression->id << "{\nlabel = \"\"\n" << this->expression->toGraphRec() << "}\n";
  ss << this->id << " -- {" << this->expression->id << "};\n";
  return ss.str();
}

CompoundStatement::CompoundStatement(int id, std::vector<ASTNode *> items) : ASTNode(id, "compound-statement") {
  this->items = std::move(items);
}

std::string CompoundStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *c : this->items) {
    ss << "subgraph cluster_" << c->id << "{\nlabel=\"\"\n" << c->toGraphRec() << "}\n";
    ss << this->id << " -- " << c->id << ";\n";
  }
  return ss.str();
}

IfStatement::IfStatement(int id, ASTNode *condition, ASTNode *if_branch, ASTNode *else_branch) : ASTNode(
    id,
    "selection-statement") {
  this->condition = condition;
  this->if_branch = if_branch;
  this->else_branch = else_branch;
}

IfStatement::IfStatement(int id, ASTNode *condition, ASTNode *if_branch) : ASTNode(id,
                                                                                   "selection-statement") {
  this->condition = condition;
  this->if_branch = if_branch;
}

std::string IfStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->condition->id << "{\nlabel = \"if\"\n" << this->condition->toGraphRec() << "}\n";
  ss << this->id << " -- " << this->condition->id << ";\n";
  ss << "subgraph cluster_" << this->if_branch->id << "{\nlabel = \"then\"\n" << this->if_branch->toGraphRec() << "}\n";
  ss << this->id << " -- " << this->if_branch->id << ";\n";
  if (this->else_branch) {
    ss << "subgraph cluster_" << this->else_branch->id << "{\nlabel = \"else\"\n" << this->else_branch->toGraphRec()
       << "}\n";
    ss << this->id << " -- " << this->else_branch->id << ";\n";
  }
  return ss.str();
}

WhileStatement::WhileStatement(int id, ASTNode *condition, ASTNode *body) : ASTNode(id, "iteration-statement") {
  this->condition = condition;
  this->body = body;
}

std::string WhileStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->condition->id << "{\nlabel = \"while\"\n" << this->condition->toGraphRec()
     << "}\n";
  ss << this->id << " -- " << this->condition->id << ";\n";
  ss << "subgraph cluster_" << this->body->id << "{\nlabel = \"do\"\n" << this->body->toGraphRec() << "}\n";
  ss << this->id << " -- " << this->body->id << ";\n";
  return ss.str();
}

GotoStatement::GotoStatement(int id, ASTNode *target) : ASTNode(id, "jump-statement") {
  this->target = target;
}

std::string GotoStatement::toGraphRec() {
  return ASTNode::toGraphRec();
}

std::string SwitchStatement::toGraphRec() {
  return ASTNode::toGraphRec();
}

SwitchStatement::SwitchStatement(int id, ASTNode *expression, ASTNode *statement) : ASTNode(id, "selection-statement") {
  this->expression = expression;
  this->statement = statement;
}
ReturnStatement::ReturnStatement(int id, ASTNode *expression) : ASTNode(id, "jump-statement") {
  this->expression = expression;
}

std::string ReturnStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->expression->id << "{\nlabel = \"\"\n" << this->expression->toGraphRec() << "}\n";
  ss << this->id << " -- {" << this->expression->id << "};\n";
  return ss.str();
}
