// lex.yy.h generated by reflex 1.0.9 from reflex_lexer.l

#ifndef REFLEX_HEADER_H
#define REFLEX_HEADER_H
#define IN_HEADER 1

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_fast                true
#define REFLEX_OPTION_header_file         lex.yy.h
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               Lexer
#define REFLEX_OPTION_outfile             lex.yy.cpp

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top{ user code %}                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include <cstdio>
#include <iostream>
#include <iomanip>
#include "token.hpp"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  REGEX MATCHER                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "../reflex/matcher.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ABSTRACT LEXER CLASS                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "../reflex/abslexer.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  LEXER CLASS                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class ReflexLexer : public reflex::AbstractLexer<reflex::Matcher> {

  std::vector<Token, std::allocator<Token>> token_list;
public:
  std::vector<Token, std::allocator<Token>> results() {
    return token_list;
  }

public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  ReflexLexer(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
      :
      AbstractBaseLexer(input, os)
  {

    token_list = std::vector<Token>();

  }
  static const int INITIAL = 0;
  virtual int lex();
  int lex(
      const reflex::Input& input,
      std::ostream        *os = NULL)
  {
    in(input);
    if (os)
      out(*os);
    return lex();
  }
};

#endif
