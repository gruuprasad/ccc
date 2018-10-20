#include <iostream>
#include <fstream>
#include <sstream>
#include "entry_point_handler.hpp"
#include "../lexer/lexer.hpp"

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if(argCount == 3 && std::string(ppArgs[1]) == "--tokenize"){
    auto filename = std::string(ppArgs[2]);
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    const auto content = buffer.str();
    const auto token_list = Lexer().lex(content);
    for (const auto &token : token_list){
      std::cout << filename << ":" << token << std::endl;
    }
    return 0;
  }
  std::cerr << "TODO: Implement a compiler." << std::endl;
  return EXIT_FAILURE;
}


