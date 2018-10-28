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

void Statement::toGraph() {

  std::cout << "graph ast {\n" << Statement::toGraphRec() << "}\n";
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

BlockStatement::BlockStatement(int id) : Statement(id, "BlockStatement") {

}

EmptyStatement::EmptyStatement(int id) : Statement(id, "EmptyStatement") {

}

DeclarationStatement::DeclarationStatement(int id) : Statement(id, "DeclarationStatement") {

}

IfStatement::IfStatement(int id) : Statement(id, "IfStatement") {

}

BreakStatement::BreakStatement(int id) : Statement(id, "BreakStatement") {

}

ExpressionStatement::ExpressionStatement(int id) : Statement(id, "ExpressionStatement") {

}

ContinueStatement::ContinueStatement(int id) : Statement(id, "ContinueStatement") {

}

WhileStatement::WhileStatement(int id) : Statement(id, "WhileStatement") {

}

ReturnStatement::ReturnStatement(int id) : Statement(id, "ReturnStatement") {

}

ErrorStatement::ErrorStatement(int id) : Statement(id, "ErrorStatement") {

}
