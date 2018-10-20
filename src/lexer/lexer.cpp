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
  std::vector<std::pair<std::string, TokenType >> v
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
          {"[a-z]+", TokenType::IDENTIFIER},
      };

  std::string reg;

  for (auto const &x : v)
    reg += "(" + x.first + ")|"; // parenthesize the submatches

  reg.pop_back();

  std::regex re(reg);
  auto words_begin = std::sregex_iterator(input.begin(), input.end(), re);
  auto words_end = std::sregex_iterator();

  unsigned long line = 0;
  unsigned long column = 0;

  auto token_list = std::list<Token>();

  for (auto it = words_begin; it != words_end; ++it) {
    size_t index = 0;

    for (; index < it->size(); ++index)
      if (!it->str(index + 1).empty()) // determine which submatch was matched
        break;

    if (v[index].second == TokenType::WHITESPACE) {
      column += it->str().size();
      continue;
    }
    if (v[index].second == TokenType::LINE_BREAK) {
      line += 1;
      column = 0;
      continue;
    }

    const Token token = Token(v[index].second, line, column, it->str());
    token_list.push_back(token);

    column += it->str().size();
  }
  return token_list;
}
