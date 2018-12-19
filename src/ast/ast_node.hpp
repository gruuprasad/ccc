#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include "../lexer/token.hpp"
#include <functional>
#include <string>
#include <vector>

class ASTNode {
protected:
  ASTNode(int id, std::string name, ccc::Token *token, unsigned long size);
  ASTNode(int id, std::string name, unsigned long size);
  ASTNode(int id, std::string name);
  ccc::Token *token;
  int id;
  std::string name;
  std::vector<ASTNode *> children;

public:
  int getId() const;
  virtual std::string toGraphWalker() = 0;
  std::string toGraph();
  virtual ~ASTNode();
};

#endif // C4_ASTNODE_HPP
