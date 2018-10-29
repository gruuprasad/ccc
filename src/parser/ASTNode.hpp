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
public:
  int getId() const;
  ASTNode(int id, std::string name, Token *token);
  ASTNode(int id, std::string name);
  virtual std::string toGraphRec();
  std::string toGraph();
};

#endif //C4_ASTNODE_HPP
