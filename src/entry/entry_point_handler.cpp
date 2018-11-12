#include <iostream>
#include <fstream>
#include <sstream>
#include "entry_point_handler.hpp"
#include "../lexer/fast_lexer.hpp"
#include "../parser/fast_parser.hpp"

using namespace ccc;

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::tokenize(std::ifstream file, const std::string &filename, std::ostream &output) {
  std::vector<Token, std::allocator<Token>> token_list;
  std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  auto lexer = FastLexer(buffer);
  token_list = lexer.lex();
  if (lexer.fail()) {
    std::cerr << filename << ":" << lexer.getError() << std::endl;
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
    auto lexer = FastLexer(buffer);
    token_list = lexer.lex();
    if (lexer.fail()) {
      std::cerr << filename << ":" << lexer.getError() << std::endl;
    return 1;
    }
    if (flagName == "--tokenize") {
      for (const auto &token : token_list) {
        std::cout << filename << ":" << token << '\n';
      }
      return 0;
    }
    if (flagName == "--parse") {
      auto parser = FastParser(token_list);
      if (parser.fail()) {
        std::cerr << filename << ":" << parser.getError() << std::endl;
        return 1;
      }
      return 0;
    }
  }
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}
