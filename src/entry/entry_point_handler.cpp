#include <iostream>
#include <fstream>
#include "entry_point_handler.hpp"
#include "../lexer/lexer.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::tokenize(std::ifstream file, const std::string &filename, std::ostream& output) {
  const auto token_list = Lexer().lex(file);
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
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}
