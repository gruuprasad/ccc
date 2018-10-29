#ifndef C4_STATEMENT_HPP
#define C4_STATEMENT_HPP

#include <vector>
#include <iosfwd>
#include <iostream>

#include "expressions.hpp"
#include "../lexer/token.hpp"

class Statement : public ASTNode {
private:
public:
  Statement(int id, const std::string &name);
  Statement(int id, const std::string &name, Token *token);
private:
  std::string toGraphRec() override;
};

class Declaration : public Statement {
};

class LabelStatement : public Statement {
public:
  LabelStatement(int id, ASTNode *identifier, ASTNode *statement);
};

class CaseStatement : public Statement {
public:
  CaseStatement(int id, ASTNode *expression, ASTNode *statement);
};

class DefaultStatement : public Statement {
public:
  DefaultStatement(int id, ASTNode *statement);
};

class CompoundStatement : public Statement {
public:
  CompoundStatement(int id, std::vector<ASTNode *> items);
};

class ExpressionStatement : public Statement {
public:
  ExpressionStatement(int id, ASTNode *expression);
  ExpressionStatement(int id, Token *token, ASTNode *expression);
};

class IfStatement : public Statement {
public:
  IfStatement(int id, ASTNode *expression, ASTNode *statement_1, ASTNode *statement_2);
  IfStatement(int id, ASTNode *expression, ASTNode *statement_1);
};

class SwitchStatement : public Statement {
public:
  SwitchStatement(int id, ASTNode *expression, ASTNode *statement);
};

class WhileStatement : public Statement {
private:
  bool post_test_loop;
public:
  WhileStatement(int id, ASTNode *expression, ASTNode *statement);
  WhileStatement(int id, ASTNode *expression, ASTNode *statement, bool post_test_loop);
};

class ForStatement : public Statement {
public:
  ForStatement(int id, ASTNode* clause_1, ASTNode* expression_2, ASTNode* expression_3, ASTNode *statement);
};

class GotoStatement : public Statement {
public:
  GotoStatement(int id, ASTNode *identifier);
};

class BreakStatement : public Statement {
public:
  explicit BreakStatement(int id);
};

class ContinueStatement : public Statement {
public:
  explicit ContinueStatement(int id);
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(int id, ASTNode *expression);
};

#endif // C4_STATEMENT_HPP
