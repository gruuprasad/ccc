#include <utility>

#include <utility>
#include <sstream>

#include "statements.hpp"

int unique = -1;

Statement::Statement(int id, std::string name) {

  Statement::id = id;
  Statement::name = std::move(name);
}

void Statement::print() {

  std::cout << Statement::name << "[";
  for (Statement *c : Statement::children) {
    c->print();
  }
  std::cout << "]";
}

std::string Statement::toGraph() {

  return "graph ast {\n" + Statement::toGraphRec() + "}\n";
}

std::string Statement::toGraphRec() {

  std::stringstream tmp;
  if (!Statement::children.empty()) {
    tmp << "\"" << Statement::id << "_" << Statement::name << "\"" << " -- {";
    for (long i = Statement::children.size(); i > 0; i--) {
      Statement *c = Statement::children[Statement::children.size() - i];
      tmp << "\"" << c->id << "_" << c->name << "\"";
      if (i > 1)
        tmp << ", ";
    }
    tmp << "};\n";
    for (Statement *c : Statement::children) {
      tmp << c->toGraphRec();
    }
  } else {
    tmp << "\"" << Statement::id << "_" << Statement::name << "\"" << ";\n";
  }
  return tmp.str();
}

void Statement::addChild(Statement *child) {

  Statement::children.emplace_back(child);
}

LabeledStatement::LabeledStatement(int id) : Statement(id, "labeled-statement") {

}

CompoundStatement::CompoundStatement(int id) : Statement(id, "compound-statement") {

}

ExpressionStatement::ExpressionStatement(int id) : Statement(id, "expression-statement") {

}

SelectionStatement::SelectionStatement(int id) : Statement(id, "selection-statement") {

}

IterationStatement::IterationStatement(int id) : Statement(id, "iteration-statement") {

}

JumpStatement::JumpStatement(int id) : Statement(id, "jump-statement") {

}
