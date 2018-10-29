#include <sstream>
#include "statement.hpp"

Statement::Statement(int id, const std::string &name, const unsigned long size) : ASTNode(id, name, size) {}

Statement::Statement(int id, const std::string &name, Token *token, const unsigned long size) : ASTNode(id,
    name,
    token,
    size) {}

std::string Statement::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=\"" << this->name << "\" shape=box style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *child : this->children) {
    ss << "subgraph cluster_" << child->getId() << "{\n" << child->toGraphWalker() << "}\n";
    ss << this->id << "--" << child->getId() << ";\n";
  }
  return ss.str();
}

Statement::Statement(int id, const std::string &name) : ASTNode(id, name) {
}

LabelStatement::LabelStatement(int id, ASTNode *identifier, ASTNode *statement) : Statement(id,
    "labeled-statement",
    2) {
  this->children.push_back(identifier);
  this->children.push_back(statement);
}

CaseStatement::CaseStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id, "labeled-statement", 2) {
  this->children.push_back(expression);
  this->children.push_back(statement);
}

DefaultStatement::DefaultStatement(int id, ASTNode *statement) : Statement(id, "labeled-statement", 1) {
  this->children.push_back(statement);
}

CompoundStatement::CompoundStatement(int id, std::vector<ASTNode *> items) : Statement(id, "compound-statement", 1) {
  this->children = std::move(items);
}

ExpressionStatement::ExpressionStatement(int id, ASTNode *expression) : Statement(id, "expression-statement", 1) {
  this->children.push_back(expression);
}

ExpressionStatement::ExpressionStatement(int id, Token *token, ASTNode *expression) : Statement(id,
    "expression-statement",
    token,
    1) {
  this->children.push_back(expression);
}

IfStatement::IfStatement(int id, ASTNode *expression, ASTNode *statement_1, ASTNode *statement_2) : Statement(id,
    "selection-statement",
    3) {
  this->children.push_back(expression);
  this->children.push_back((statement_1));
  this->children.push_back(statement_2);
}

IfStatement::IfStatement(int id, ASTNode *expression, ASTNode *statement_1) : Statement(id, "selection-statement", 2) {
  this->children.push_back(expression);
  this->children.push_back(statement_1);
}

SwitchStatement::SwitchStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id,
    "selection-statement",
    2) {
  this->children.push_back(expression);
  this->children.push_back((statement));
}

WhileStatement::WhileStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id,
    "iteration-statement",
    2) {
  this->children.push_back(expression);
  this->children.push_back((statement));
}

DoStatement::DoStatement(int id, ASTNode *expression, ASTNode *statement) : Statement(id, "iteration-statement", 2) {
  this->children.push_back(expression);
  this->children.push_back(statement);
}

ForStatement::ForStatement(int id, ASTNode *clause_1, ASTNode *expression_2, ASTNode *expression_3, ASTNode *statement)
    : Statement(id, "iteration-statement", 4) {
  this->children.push_back(clause_1);
  this->children.push_back(expression_2);
  this->children.push_back(expression_3);
  this->children.push_back(statement);
}

GotoStatement::GotoStatement(int id, ASTNode *identifier) : Statement(id, "jump-statement", 1) {
  this->children.push_back(identifier);
}

BreakStatement::BreakStatement(int id) : Statement(id, "jump-statement") {
}

ContinueStatement::ContinueStatement(int id) : Statement(id, "jump-statement") {
}

ReturnStatement::ReturnStatement(int id, ASTNode *expression) : Statement(id, "jump-statement", 1) {
  this->children.push_back(expression);
}
