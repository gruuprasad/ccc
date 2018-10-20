#include <iostream>
#include <vector>
#include <regex>
#include "lexer.hpp"
#include "token_type.hpp"

Lexer::Lexer() = default;

int Lexer::lex(std::string input) {
  // use std::vector instead, we need to have it in this order
  std::vector<std::pair<std::string, std::string>> v
      {
          {"[0-9]+" , "NUMBERS"} ,
          {"[a-z]+" , "IDENTIFIERS"},
          {"\\*", "STAR"},
          {"\\+", "PLUS"},
          {"\\-", "MINUS"},
          {"\\{", "BRACE_OPEN"},
          {"\\}", "BRACE_CLOSE"},
          {"\n", "LINE_BREAK"},
          {"[ \t]+", "WHITESPACE"},
      };

  std::string reg;

  for(auto const& x : v)
    reg += "(" + x.first + ")|"; // parenthesize the submatches

  reg.pop_back();
  std::cout << reg << std::endl;

  std::regex re(reg);
  auto words_begin = std::sregex_iterator(input.begin(), input.end(), re);
  auto words_end = std::sregex_iterator();

  unsigned long line = 0;
  unsigned long column = 0;

  for(auto it = words_begin; it != words_end; ++it)
  {
    size_t index = 0;

    for( ; index < it->size(); ++index)
      if(!it->str(index + 1).empty()) // determine which submatch was matched
        break;

    if(v[index].second == "WHITESPACE"){
      column += it->str().size();
      continue;
    }
    if(v[index].second == "LINE_BREAK"){
      line += 1;
      column = 0;
      continue;
    }

    std::cout << line << ',' << column << " \t" << it->str() << "\t" << v[index].second << std::endl;
    column += it->str().size();
  }
  std::cout << input << std::endl;
  return 0;
}
