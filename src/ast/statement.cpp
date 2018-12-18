#include <utility>

#include <utility>

#include "statement.hpp"
#include <sstream>

Statement::Statement(int id, const std::string &name, const unsigned long size)
    : ASTNode(id, name, size) {}

std::string Statement::toGraphWalker() {
  std::stringstream ss;
  ss << this->id << "[label=<" << this->name << "<br/><font point-size='10'>"
     << this->name
     << "</font>> shape=invhouse style=filled fillcolor=mediumaquamarine];\n";
  for (ASTNode *child : this->children) {
    ss << "subgraph cluster_" << child->getId() << "{\n"
       << child->toGraphWalker() << "}\n";
    ss << this->id << "--" << child->getId() << ";\n";
  }
  return ss.str();
}

Statement::Statement(int id, const std::string &name) : ASTNode(id, name) {}

LabelStatement::LabelStatement(int id, Expression *identifier,
                               Statement *statement)
    : Statement(id, "labeled-statement", 2) {
  this->children.push_back(identifier);
  this->children.push_back(statement);
}

CompoundStatement::CompoundStatement(int id, std::vector<ASTNode *> items)
    : Statement(id, "compound-statement", 1) {
  this->children = std::move(items);
}

ExpressionStatement::ExpressionStatement(int id, Expression *expression)
    : Statement(id, "expression-statement", 1) {
  this->children.push_back(expression);
}

ExpressionStatement::ExpressionStatement(int id)
    : Statement(id, "expression-statement") {}

IfStatement::IfStatement(int id, Expression *expression, Statement *statement_1)
    : Statement(id, "selection-statement", 2) {
  this->children.push_back(expression);
  this->children.push_back(statement_1);
}

IfElseStatement::IfElseStatement(int id, Expression *expression,
                                 Statement *statement_1, Statement *statement_2)
    : Statement(id, "selection-statement", 3) {
  this->children.push_back(expression);
  this->children.push_back(statement_1);

  this->children.push_back(statement_2);
}

WhileStatement::WhileStatement(int id, Expression *expression,
                               Statement *statement)
    : Statement(id, "iteration-statement", 2) {
  this->children.push_back(expression);
  this->children.push_back((statement));
}

GotoStatement::GotoStatement(int id, Expression *identifier)
    : Statement(id, "jump-statement", 1) {
  this->children.push_back(identifier);
}

BreakStatement::BreakStatement(int id) : Statement(id, "jump-statement") {}

ContinueStatement::ContinueStatement(int id)
    : Statement(id, "jump-statement") {}

ReturnStatement::ReturnStatement(int id, Expression *expression)
    : Statement(id, "jump-statement", 1) {
  this->children.push_back(expression);
}
