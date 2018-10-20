#include <iostream>
#include <vector>
#include <regex>
#include <list>
#include "lexer.hpp"
#include "token_type.hpp"
#include "token.hpp"

Lexer::Lexer() = default;

std::list<Token, std::allocator<Token>> Lexer::lex(std::string input) {
  // use std::vector instead, we need to have it in this order
  std::vector<std::pair<std::string, TokenType >> keywords
      {
          {"auto", TokenType::AUTO},
          {"break", TokenType::BREAK},
          {"case", TokenType::CASE},
          {"char", TokenType::CHAR},
          {"const", TokenType::CONST},
          {"continue", TokenType::CONTINUE},
          {"default", TokenType::DEFAULT},
          {"do", TokenType::DO},
          {"else", TokenType::ELSE},
          {"enum", TokenType::ENUM},
          {"extern", TokenType::EXTERN},
          {"for", TokenType::FOR},
          {"goto", TokenType::GOTO},
          {"if", TokenType::IF},
          {"inline", TokenType::INLINE},
          {"int", TokenType::INT},
          {"long", TokenType::LONG},
          {"register", TokenType::REGISTER},
          {"restrict", TokenType::RESTRICTED},
          {"return", TokenType::RETURN},
          {"short", TokenType::SHORT},
          {"signed", TokenType::SIGNED},
          {"sizeof", TokenType::SIZEOF},
          {"static", TokenType::STATIC},
          {"struct", TokenType::STRUCT},
          {"switch", TokenType::SWITCH},
          {"typedef", TokenType::TYPEDEF},
          {"union", TokenType::UNION},
          {"unsigned", TokenType::UNSIGNED},
          {"void", TokenType::VOID},
          {"volatile", TokenType::VOLATILE},
          {"while", TokenType::WHILE},
          {"_Alignas", TokenType::ALIGN_AS},
          {"_Alignof", TokenType::ALIGN_OF},
          {"_Atomic", TokenType::ATOMIC},
          {"_Bool", TokenType::BOOL},
          {"_Complex", TokenType::COMPLEX},
          {"_Generic", TokenType::GENERIC},
          {"_Imaginary", TokenType::IMAGINARY},
          {"_Noreturn", TokenType::NO_RETURN},
          {"_Static_assert", TokenType::STATIC_ASSERT},
          {"_Thread_local", TokenType::THREAD_LOCAL},
      };

  std::vector<std::pair<std::string, TokenType >> all
      {
          {"[a-zA-Z_][a-zA-Z_0-9]*", TokenType::IDENTIFIER},
          {"\\<\\<", TokenType::LEFT_SHIFT},
          {"\\>\\>", TokenType::RIGHT_SHIFT},
          {"\\>\\=", TokenType::GREATER_EQUAL},
          {"\\<\\=", TokenType::LESS_EQUAL},
          {"\\=\\=", TokenType::EQUAL},
          {"\\=", TokenType::ASSIGN},
          {"\\+\\=", TokenType::PLUS_ASSIGN},
          {"\\-\\=", TokenType::MINUS_ASSIGN},
          {"\\&\\=", TokenType::AMPERSAND_ASSIGN},
          {"\\|\\=", TokenType::PIPE_ASSIGN},
          {"\\^\\=", TokenType::CARET_ASSIGN},
          {"\\~\\=", TokenType::TILDE_ASSIGN},
          {"\\*\\=", TokenType::STAR_ASSIGN},
          {"\\/\\=", TokenType::DIV_ASSIGN},
          {"\\%\\=", TokenType::MOD_ASSIGN},
          {"\\*", TokenType::STAR},
          {"\\&", TokenType::AMPERSAND},
          {"\\|", TokenType::PIPE},
          {"\\^", TokenType::CARET},
          {"\\~", TokenType::TILDE},
          {"\\/", TokenType::DIV},
          {"\\%", TokenType::MOD},
          {"\\^", TokenType::CARET},
          {"\\+\\+", TokenType::PLUSPLUS},
          {"\\+", TokenType::PLUS},
          {"\\-\\-", TokenType::MINUSMINUS},
          {"\\-", TokenType::MINUS},
          {"\\{", TokenType::BRACE_OPEN},
          {"\\}", TokenType::BRACE_CLOSE},
          {"\\[", TokenType::BRACKET_OPEN},
          {"\\]", TokenType::BRACKET_CLOSE},
          {"\\(", TokenType::PARENTHESIS_OPEN},
          {"\\)", TokenType::PARENTHESIS_CLOSE},
          {"\n", TokenType::LINE_BREAK},
          {"[ \t]+", TokenType::WHITESPACE},
          {"[0-9]+", TokenType::NUMBER},
      };

  all.insert(all.end(), keywords.begin(), keywords.end());

  std::string all_regex_string;
  for (auto const &x : all) {
    all_regex_string += "(" + x.first + ")|"; // parenthesize the submatches
  }
  all_regex_string.pop_back();

  std::string keywords_regex_string;
  for (auto const &x : keywords) {
    keywords_regex_string += "(" + x.first + ")|"; // parenthesize the submatches
  }
  keywords_regex_string.pop_back();

  std::regex all_regex(all_regex_string);
  std::regex keywords_regex(keywords_regex_string);
  auto words_begin = std::sregex_iterator(input.begin(), input.end(), all_regex);
  auto words_end = std::sregex_iterator();

  unsigned long line = 0;
  unsigned long column = 0;

  auto token_list = std::list<Token>();
  TokenType token_type;
  size_t index = 0;
  for (auto it = words_begin; it != words_end; ++it) {
    index = 0;
    for (; index < it->size(); ++index) {
      // determine which submatch was matched
      if (!it->str(index + 1).empty()) {
        break;
      }
    }

    token_type = all[index].second;
    const std::string &match = it->str();

    if (token_type == TokenType::WHITESPACE) {
      column += match.size();
      continue;
    }
    if (token_type == TokenType::LINE_BREAK) {
      line += 1;
      column = 0;
      continue;
    }

    if (token_type == TokenType::IDENTIFIER) {
      for( auto keyword : keywords){
        if(keyword.first == match){
          token_type = keyword.second;
        }
      }
    }

    const Token token = Token(token_type, line, column, match);
    token_list.push_back(token);

    column += match.size();
  }
  return token_list;
}
