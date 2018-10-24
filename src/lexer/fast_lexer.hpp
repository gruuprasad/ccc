#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

static const char *const AUTO = "auto";
static const char *const BREAK = "break";
static const char *const CASE = "case";
static const char *const CHAR = "char";
static const char *const CONST = "const";
static const char *const CONTINUE = "continue";
static const char *const DO = "do";
static const char *const DEFAULT = "default";
static const char *const ELSE = "else";
static const char *const ENUM = "enum";
static const char *const EXTERN = "extern";
static const char *const FOR = "for";
static const char *const GOTO = "goto";
static const char *const IF = "if";
static const char *const INT = "int";
static const char *const INLINE = "inline";
static const char *const LONG = "long";
static const char *const REGISTER = "register";
static const char *const RESTRICT = "restrict";
static const char *const RETURN = "return";
static const char *const SHORT = "short";
static const char *const SIGNED = "signed";
static const char *const SIZEOF = "sizeof";
static const char *const STATIC = "static";
static const char *const STRUCT = "struct";
static const char *const SWITCH = "switch";
static const char *const TYPEDEF = "typedef";
static const char *const UNION = "union";
static const char *const UNSIGNED = "unsigned";
static const char *const VOID = "void";
static const char *const VOLATILE = "volatile";
static const char *const WHILE = "while";
static const char *const ALIGN_AS = "_Alignas";
static const char *const ALIGN_OF = "_Alignof";
static const char *const ATOMIC = "_Atomic";
static const char *const BOOL = "_Bool";
static const char *const COMPLEX = "_Complex";
static const char *const GENERIC = "_Generic";
static const char *const IMAGINARY = "_Imaginary";
static const char *const NO_RETURN = "_Noreturn";
static const char *const STATIC_ASSERT = "_Static_assert";
static const char *const THREAD_LOCAL = "_Thread_local";
#include <list>
#include <vector>
#include "token.hpp"
#include "lexer_exception.hpp"

class FastLexer {
  std::vector<Token, std::allocator<Token>> token_list;
  const std::string content;
  unsigned long position;
  unsigned long line;
  unsigned long column;
  inline bool munch();
  inline char getCharAt(unsigned long position);
  inline bool keyWordEnd(unsigned long position);
  inline bool isKeyword();
  inline bool isPunctuator();
public:
  FastLexer(const std::string &content);
  std::vector<Token, std::allocator<Token>> lex();
};

#endif //C4_FASTLEXER_H
