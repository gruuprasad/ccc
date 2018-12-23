#include "entry_point_handler.hpp"
#include "../lexer/fast_lexer.hpp"
#include "../parser/fast_parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace ccc;

EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 3) {
    const std::string flagName = std::string(ppArgs[1]);
    std::string path = ppArgs[2];
    std::ifstream file = std::ifstream(path);
    auto filename = path.substr(path.find_last_of("/\\") + 1);
    std::vector<Token, std::allocator<Token>> token_list;
    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    auto lexer = FastLexer(buffer);
    token_list = lexer.lex();
    if (lexer.fail()) {
      std::cerr << filename << ":" << lexer.getError() << std::endl;
      return EXIT_FAILURE;
    }
    if (flagName == "--tokenize") {
      for (const auto &token : token_list) {
        std::cout << filename << ":" << token << '\n';
      }
      return EXIT_SUCCESS;
    }
    if (flagName == "--parse") {
      token_list.emplace_back(TokenType::ENDOFFILE, -1, -1);
      auto parser = FastParser(token_list);
      parser.parse();
      if (parser.fail()) {
        std::cerr << filename << ":" << parser.getError() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}
