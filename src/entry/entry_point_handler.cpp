#include <ios>
#include <iostream>
#include <fstream>
#include <streambuf>
#include "entry_point_handler.hpp"
#include "../lexer/lexer_exception.hpp"
#include "../lexer/fast_lexer.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::tokenize(std::ifstream file, const std::string &filename, std::ostream& output) {
  std::ios_base::sync_with_stdio(false);
  std::vector<Token, std::allocator<Token>> token_list;
  std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  try {
    token_list = FastLexer(buffer).lex();
  } catch (LexerException &exception){
    std::cerr << filename << ":" << exception.what() << std::endl;
    return 1;
  }
  for (const auto &token : token_list) {
    output << filename << ":" << token << '\n';
  }
  return 0;
}

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3 && std::string(ppArgs[1]) == "--tokenize") {
    auto filename = ppArgs[2];
    return tokenize(std::ifstream(filename), filename, std::cout);
  }
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}
