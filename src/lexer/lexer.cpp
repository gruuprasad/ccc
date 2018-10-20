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
          {"[0-9]+", TokenType::NUMBER},
          {"[a-z]+", TokenType::IDENTIFIER},
          {"\\*", TokenType::STAR},
          {"\\+", TokenType::PLUS},
          {"\\-", TokenType::MINUS},
          {"\\{", TokenType::BRACE_OPEN},
          {"\\}", TokenType::BRACE_CLOSE},
          {"\n", TokenType::LINE_BREAK},
          {"[ \t]+", TokenType::WHITESPACE},
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
