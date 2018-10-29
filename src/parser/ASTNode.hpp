#ifndef C4_ASTNODE_HPP
#define C4_ASTNODE_HPP

#include <string>
#include <vector>
#include <functional>
#include "../lexer/token.hpp"

class ASTNode {
protected:
  Token *token;
  int id;
  std::string name;
  std::vector<ASTNode*> children;
public:
  int getId() const;
  ASTNode* getChild(int pos);
  ASTNode(int id, std::string name, Token *token);
  ASTNode(int id, std::string name);
  virtual std::string toGraphRec();
  std::string toGraph();
  virtual ~ASTNode();
};

#endif //C4_ASTNODE_HPP
