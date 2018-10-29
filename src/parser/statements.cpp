#include <sstream>
#include "statements.hpp"

ExpressionStatement::ExpressionStatement(int id, ASTNode *expression) : Statement(id, "expression-statement") {
  this->children.emplace_back(expression);
}

ExpressionStatement::ExpressionStatement(int id, Token *token, ASTNode *expression) : Statement(id,
    "expression-statement",
    token) {
  this->children.emplace_back(expression);
}

CompoundStatement::CompoundStatement(int id, std::vector<ASTNode *> items) : Statement(id, "compound-statement") {
  this->children = std::move(items);
}

IfStatement::IfStatement(int id, ASTNode *expression, ASTNode *statement_1, ASTNode *statement_2) : Statement(id,
    "selection-statement") {
  this->children.emplace_back(expression);
  this->children.emplace_back((statement_1));
  this->children.emplace_back(statement_2);
}

IfStatement::IfStatement(int id, ASTNode *expression, ASTNode *statement_1) : Statement(id, "selection-statement") {
  this->children.emplace_back(expression);
  this->children.emplace_back((statement_1));
}

WhileStatement::WhileStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id, "iteration-statement") {
  this->children.emplace_back(expression);
  this->children.emplace_back((statement));
  this->post_test_loop = false;
}

std::string Statement::toGraphRec() {
  std::stringstream ss;
  ss << this->getId() << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *c : this->children) {
    ss << "subgraph cluster_" << c->getId() << "{\n" << c->toGraphRec() << "}\n";
    ss << this->getId() << " -- " << c->getId() << ";\n";
  }
  return ss.str();
}

WhileStatement::WhileStatement(int id, ASTNode *expression, ASTNode *statement, bool post_test_loop) : Statement(id,
    "iteration-statement") {
  this->children.emplace_back(expression);
  this->children.emplace_back(statement);
  this->post_test_loop = post_test_loop;
}

GotoStatement::GotoStatement(int id, ASTNode *identifier) : Statement(id, "jump-statement") {
  this->children.emplace_back(identifier);
}

SwitchStatement::SwitchStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id,
    "selection-statement") {
  this->children.emplace_back(expression);
  this->children.emplace_back((statement));
}

ReturnStatement::ReturnStatement(int id, ASTNode *expression) : Statement(id, "jump-statement") {
  this->children.emplace_back(expression);
}

BreakStatement::BreakStatement(int id) : Statement(id, "jump-statement") {
}

ContinueStatement::ContinueStatement(int id) : Statement(id, "jump-statement") {
}

Statement::Statement(int id, const std::string &name) : ASTNode(id, name) {}

Statement::Statement(int id, const std::string &name, Token *token) : ASTNode(id, name, token) {}

LabelStatement::LabelStatement(int id, ASTNode *identifier, ASTNode *statement) : Statement(id, "labeled-statement") {
  this->children.emplace_back(identifier);
  this->children.emplace_back(statement);
}

CaseStatement::CaseStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id, "labeled-statement") {
  this->children.emplace_back(expression);
  this->children.emplace_back(statement);
}

DefaultStatement::DefaultStatement(int id, ASTNode *statement) : Statement(id, "labeled-statement") {
  this->children.emplace_back(statement);
}

ForStatement::ForStatement(int id, ASTNode *clause_1, ASTNode *expression_2, ASTNode *expression_3, ASTNode *statement)
    : Statement(id, "iteration-statement") {
  this->children.emplace_back(clause_1);
  this->children.emplace_back(expression_2);
  this->children.emplace_back(expression_3);
  this->children.emplace_back(statement);
}
