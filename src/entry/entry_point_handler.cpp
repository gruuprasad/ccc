#include "entry_point_handler.hpp"
#include "../ast/visitor/codegen.hpp"
#include "../ast/visitor/graphviz.hpp"
#include "../ast/visitor/semantic_analysis.hpp"
#include "../lexer/fast_lexer.hpp"
#include "../parser/fast_parser.hpp"

#define PARSE                                                                  \
  auto parser = FastParser(buffer, path);                                      \
  auto root = parser.parse();                                                  \
  if (parser.fail()) {                                                         \
    std::cerr << parser.getError() << std::endl;                               \
    return EXIT_FAILURE;                                                       \
  }
#define SEMAN                                                                  \
  SemanticVisitor sv;                                                          \
  root->accept(&sv);                                                           \
  if (sv.fail()) {                                                             \
    std::cerr << path << ":" << sv.getError() << std::endl;                    \
    return EXIT_FAILURE;                                                       \
  }
#define COMPILE                                                                \
  CodegenVisitor cv(path);                                                     \
  root->accept(&cv);                                                           \
  cv.compile();
#define HELP                                                                   \
  std::cout                                                                    \
      << "Usage: c4 [options] file\n"                                          \
         "Options:\n"                                                          \
         "  --tokenize                perform lexical analysis and print "     \
         "token list\n"                                                        \
         "  --parse                   tokenize, parse and perform semantical " \
         "analysis\n"                                                          \
         "  --print-ast               like --parse but pretty print from "     \
         "AST\n"                                                               \
         "  --graphviz                like --parse but print graphviz "        \
         "representation of AST\n"                                             \
         "  --compile                 compile to LLVM IR\n"                    \
         "  --optimize                WIP\n"                                   \
         "  --optimize-run-time       WIP\n"                                   \
         "  --optimize-compile-time   WIP\n"                                   \
      << std::endl;
namespace ccc {
EntryPointHandler::EntryPointHandler() = default;

int EntryPointHandler::handle(int argCount, char **const ppArgs) {
  if (argCount == 2) {
    const std::string flag = std::string(ppArgs[1]);
    if (flag == "--help") {
      HELP;
      return EXIT_SUCCESS;
    }
    const auto &path = flag;
    std::ifstream file = std::ifstream(path);
    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    PARSE;
    SEMAN;
    COMPILE;
    return EXIT_SUCCESS;
  }
  if (argCount == 3) {
    const std::string flag = std::string(ppArgs[1]);
    std::string path = ppArgs[2];
    std::ifstream file = std::ifstream(path);
    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    if (flag == "--tokenize") {
      auto lexer = FastLexer(buffer, path);
      lexer.tokenize();
      if (lexer.fail()) {
        std::cerr << lexer.getError() << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    } else if (flag == "--parse") {
      PARSE;
      SEMAN;
      return EXIT_SUCCESS;
    } else if (flag == "--print-ast") {
      PARSE;
      SEMAN;
      PrettyPrinterVisitor pp;
      std::cout << root->accept(&pp);
      return EXIT_SUCCESS;
    } else if (flag == "--graphviz") {
      PARSE;
      SEMAN;
      GraphvizVisitor gv;
      std::cout << root->accept(&gv) << std::endl;
      return EXIT_SUCCESS;
    } else if (flag == "--compile") {
      PARSE;
      SEMAN;
      COMPILE;
      return EXIT_SUCCESS;
    }
  }
  HELP;
  return EXIT_SUCCESS;
}
} // namespace ccc
