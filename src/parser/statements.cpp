#include <utility>

#include <sstream>

#include "statements.hpp"

ExpressionStatement::ExpressionStatement(int id, ASTNode *expression) : ASTNode(id, "expression-statement") {
  this->expression = expression;
}

std::string ExpressionStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->expression->getId() << "{\nlabel = \"\"\n" << this->expression->toGraphRec()
     << "}\n";
  ss << this->getId() << " -- {" << this->expression->getId() << "};\n";
  return ss.str();
}
ExpressionStatement::~ExpressionStatement() {
  delete &this->expression;
}

ExpressionStatement::ExpressionStatement(int id, Token *token, ASTNode *expression) : ASTNode(id,
                                                                                              "expression-statement",
                                                                                              token) {
  this->
      expression = expression;
}

CompoundStatement::CompoundStatement(int id, std::vector<ASTNode *> items) : ASTNode(id, "compound-statement") {
  this->items = std::move(items);
}

std::string CompoundStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *c : this->items) {
    ss << "subgraph cluster_" << c->getId() << "{\nlabel=\"\"\n" << c->toGraphRec() << "}\n";
    ss << this->getId() << " -- " << c->getId() << ";\n";
  }
  return ss.str();
}
CompoundStatement::~CompoundStatement() {
  delete &this->items;
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
  this->else_branch = nullptr;
}

std::string IfStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->condition->getId() << "{\nlabel = \"if\"\n" << this->condition->toGraphRec()
     << "}\n";
  ss << this->getId() << " -- " << this->condition->getId() << ";\n";
  ss << "subgraph cluster_" << this->if_branch->getId() << "{\nlabel = \"then\"\n" << this->if_branch->toGraphRec()
     << "}\n";
  ss << this->getId() << " -- " << this->if_branch->getId() << ";\n";
  if (this->else_branch) {
    ss << "subgraph cluster_" << this->else_branch->getId() << "{\nlabel = \"else\"\n"
       << this->else_branch->toGraphRec()
       << "}\n";
    ss << this->getId() << " -- " << this->else_branch->getId() << ";\n";
  }
  return ss.str();
}
IfStatement::~IfStatement() {
  delete &this->condition;
  delete &this->if_branch;
  delete &this->else_branch;
}

WhileStatement::WhileStatement(int id, ASTNode *condition, ASTNode *body) : ASTNode(id, "iteration-statement") {
  this->condition = condition;
  this->body = body;
  this->post_test_loop = false;
}

std::string WhileStatement::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->condition->getId() << "{\nlabel = \"while\"\n" << this->condition->toGraphRec()
     << "}\n";
  ss << this->getId() << " -- " << this->condition->getId() << ";\n";
  ss << "subgraph cluster_" << this->body->getId() << "{\nlabel = \"do\"\n" << this->body->toGraphRec() << "}\n";
  ss << this->getId() << " -- " << this->body->getId() << ";\n";
  return ss.str();
}
WhileStatement::~WhileStatement() {
  delete &this->condition;
  delete &this->body;
}
WhileStatement::WhileStatement(int id, ASTNode *condition, ASTNode *body, bool post_test_loop) : ASTNode(id,
                                                                                                         "iteration-statement") {
  this->condition = condition;
  this->body = body;
  this->post_test_loop = post_test_loop;
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
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  ss << "subgraph cluster_" << this->expression->getId() << "{\nlabel = \"\"\n" << this->expression->toGraphRec()
     << "}\n";
  ss << this->getId() << " -- {" << this->expression->getId() << "};\n";
  return ss.str();
}
ReturnStatement::~ReturnStatement() {
  delete &this->expression;
}

BreakStatement::BreakStatement(int id) : ASTNode(id, "jump-statement") {
}

ContinueStatement::ContinueStatement(int id) : BreakStatement(id) {
}
