#include <iostream>
#include <fstream>
#include <sstream>
#include "entry_point_handler.hpp"
#include "../lexer/lexer.hpp"
#include "../lexer/lexer_exception.hpp"
#include "../lexer/fast_lexer.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::tokenize(std::ifstream file, const std::string &filename, std::ostream& output) {
  std::list<Token, std::allocator<Token>> token_list;
  //std::vector<Token, std::allocator<Token>> token_list;
  //std::stringstream buffer;
  //buffer << file.rdbuf();
  try {
    //token_list = FastLexer(buffer.str()).lex();
    token_list = Lexer().lex(file);
  } catch (LexerException &exception){
    std::cerr << filename << ":" << exception.what() << std::endl;
    return 1;
  }
  for (const auto &token : token_list) {
    output << filename << ":" << token << std::endl;
  }
  return 0;
}

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3 && std::string(ppArgs[1]) == "--tokenize") {
    auto filename = ppArgs[2];
    return tokenize(std::ifstream(filename), filename, std::cout);
  }
  if (argCount == 3 && std::string(ppArgs[1]) == "--tokenize-old") {
    auto filename = ppArgs[2];
    auto file = std::ifstream(filename);
    std::list<Token, std::allocator<Token>> token_list;
    try {
      token_list = Lexer().lex(file);
    } catch (LexerException &exception){
      std::cerr << filename << ":" << exception.what() << std::endl;
      return 1;
    }
    for (const auto &token : token_list) {
      std::cout << filename << ":" << token << std::endl;
    }
    return 0;
  }
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}
