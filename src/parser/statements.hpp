#ifndef C4_STATEMENT_HPP
#define C4_STATEMENT_HPP

#include <vector>
#include <iosfwd>
#include <iostream>

#include "expressions.hpp"
#include "ASTNode.hpp"
#include "../lexer/token.hpp"

class Declaration : public ASTNode {
};

class LabelStatement : public ASTNode {
};

class CaseStatement : public ASTNode {
};

class DefaultStatement : public ASTNode {
};

class CompoundStatement : public ASTNode {
private:
  std::vector<ASTNode *> items;
  std::string toGraphRec() override;
public:
  CompoundStatement(int id, std::vector<ASTNode *> items);
  virtual ~CompoundStatement();
};

class ExpressionStatement : public ASTNode {
private:
  ASTNode *expression;
  std::string toGraphRec() override;
public:
  ExpressionStatement(int id, ASTNode *expression);
  ExpressionStatement(int id, Token *token, ASTNode *expression);
  virtual ~ExpressionStatement();
};

class IfStatement : public ASTNode {
private:
  ASTNode *condition;
  ASTNode *if_branch;
  ASTNode *else_branch;
  std::string toGraphRec() override;
public:
  IfStatement(int id, ASTNode *condition, ASTNode *if_branch, ASTNode *else_branch);
  IfStatement(int id, ASTNode *condition, ASTNode *if_branch);
  virtual ~IfStatement();
};

class SwitchStatement : public ASTNode {
private:
  ASTNode *expression;
  ASTNode *statement;
  std::string toGraphRec() override;
public:
  SwitchStatement(int id, ASTNode *expression, ASTNode *statement);
};

class WhileStatement : public ASTNode {
private:
  ASTNode *condition;
  ASTNode *body;
  std::string toGraphRec() override;
  bool post_test_loop;
public:
  WhileStatement(int id, ASTNode *condition, ASTNode *body);
  WhileStatement(int id, ASTNode *condition, ASTNode *body, bool post_test_loop);
  virtual ~WhileStatement();
};

class ForStatement : public ASTNode {
};

class GotoStatement : public ASTNode {
private:
  ASTNode *target;
  std::string toGraphRec() override;
public:
  GotoStatement(int id, ASTNode *target);
};

class BreakStatement : public ASTNode {
public:
  explicit BreakStatement(int id);
};

class ContinueStatement : public BreakStatement {
public:
  explicit ContinueStatement(int id);
};

class ReturnStatement : public ASTNode {
private:
  ASTNode *expression;
  std::string toGraphRec() override;
public:
  ReturnStatement(int id, ASTNode *expression);
  virtual ~ReturnStatement();
};

#endif // C4_STATEMENT_HPP
