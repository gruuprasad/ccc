#ifndef C4_PARSER_HPP
#define C4_PARSER_HPP

#include "../lexer/token.hpp"
#include "../utils/assert.hpp"
#include <vector>

namespace ccc {

#define C_TYPES TokenType::VOID, TokenType::CHAR, TokenType::SHORT, \
              TokenType::INT, TokenType::STRUCT \
  
class FastParser {
public:
  FastParser(std::vector<Token> & tokens_) : tokens(tokens_) {}
  
  void parse() { return parseTranslationUnit(); }
  bool fail() const { return !error.empty(); }
  std::string getError() { return error; }

private:
  Token nextToken() {  
    return tokens[curTokenIdx++];
  }

  bool consume() {
    curTokenIdx++;
    return true;
  }

  bool expect(TokenType tok) {
    my_assert(tok == peek().getType()) << "Parse Error: Unexpected Token: "
                                       << peek().name();
    return consume();
  }

  const Token& peek(int k = 0) const { 
    // TODO bound check. Append k number of empty tokens at the end of tokens
    // so we don't need to bound check.
    return tokens[curTokenIdx + k]; 
  }

  template <typename F>
  void parseList(F word, TokenType delimit) {
    do {
      word();
    } while(peek().is(delimit) && consume());
  }
  
  // Grammar Rules we plan to implement.
  // (6.9) translationUnit :: external-declaration+
  // (6.9) external-declaration :: function-definition | declaration
  // (6.9.1) function-definition :: type-specifier declarator declaration+(opt) compound-statement
  // (6.7)  declaration :: type-specifier declarator(opt) ;
  // (6.7.2) type-specifier :: void | char | short | int | struct-or-union-specifier
  // (6.7.2.1) struct-or-union-specifier :: struct identifer(opt) { struct-declaration+ } | struct-or-union identifier
  // (6.7.2.1) struct-declaration :: type-specifier declarator (opt) ;
  // (6.7.6)  declarator :: pointer(opt) direct-declarator
  // (6.7.6)  direct-declarator :: identifier | ( declarator ) | direct-declarator ( parameter-list )
  // (6.7.6)  parameter-list :: (type-specifier declarator)+ (comma-separated)
  // TODO Add rules -> parameter-declaration :: type-specifiers abstract-declarator(opt)
  //                   abstract-declarator :: pointer | pointer(opt) direct-abstract-declarator
  //                   direct-abstract-declarator :: ( abstract-declarator) | ( parameter-type-list(opt) )+

  void parseTranslationUnit() { return parseExternalDeclaration(); }       
  void parseExternalDeclaration();
  void parseFuncDefOrDeclaration();     

  // declarations
  void parseTypeSpecifiers();         
  void parseDeclarator();            
  void parseDirectDeclarator();       
  void parseParameterList();
  void parseStructOrUnionSpecifier(); 
  void parseStructDeclaration();      

  // Expressions

  std::vector<Token> & tokens;
  std::size_t curTokenIdx = 0;
  std::string error = "";
};

} // namespace ccc
#endif

