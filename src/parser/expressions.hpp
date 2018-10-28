#ifndef C4_EXPRESSIONS_HPP
#define C4_EXPRESSIONS_HPP

#include <vector>
#include <iosfwd>
#include <iostream>
#include "ASTNode.hpp"

class Identifier : public ASTNode {
public:
  Identifier(int id);
  std::string toGraphRec() override;
};

class AdditiveExpression: public ASTNode {
private:
  ASTNode* left;
  ASTNode* right;
public:
  AdditiveExpression(int id, ASTNode* left, ASTNode* right);
  std::string toGraphRec() override;
};
#endif // C4_EXPRESSIONS_HPP
