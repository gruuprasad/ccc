#include <iostream>
#include <fstream>
#include <sstream>
#include "entry_point_handler.hpp"
#include "../lexer/lexer_exception.hpp"
#include "../lexer/fast_lexer.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::tokenize(std::ifstream file, const std::string &filename, std::ostream &output) {
  std::vector<Token, std::allocator<Token>> token_list;
  std::stringstream buffer;
  buffer << file.rdbuf();
  try {
    token_list = FastLexer(buffer.str()).lex();
  } catch (LexerException &exception) {
    std::cerr << filename << ":" << exception.what() << std::endl;
    return 1;
  }
  for (const auto &token : token_list) {
    output << filename << ":" << token << '\n';
  }
  return 0;
}

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3) {
    const std::string flagName = std::string(ppArgs[1]);
    auto filename = ppArgs[2];
    std::ifstream file = std::ifstream(filename);
    std::vector<Token, std::allocator<Token>> token_list;
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    try {
      token_list = FastLexer(buffer).lex();
    } catch (LexerException &exception) {
      std::cerr << filename << ":" << exception.what() << std::endl;
      return 1;
    }
    if (flagName == "--tokenize") {
      for (const auto &token : token_list) {
        std::cout << filename << ":" << token << '\n';
      }
      return 0;
    }
    if (flagName == "--parse") {
      return 0;
    }
  }
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}
