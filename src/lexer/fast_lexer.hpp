#ifndef C4_FASTLEXER_H
#define C4_FASTLEXER_H

#include "../utils/macros.hpp"
#include "token.hpp"
#include <utility>
#include <vector>

namespace ccc {

class FastLexer {
  std::string filename;
  const std::string &content;
  std::string error;
  unsigned long position = 0;
  unsigned long line = 1;
  unsigned long column = 0;
  /**
   * Munch the next token in content.
   *
   * This method is implemented as a jump table for
   * the current character and delegates to sub methods.
   *
   * Optimized for performance, not readability.
   *
   * @return return a lexed Token of said content
   */
  inline Token munch();
  /**
   * Inline alias to access the content.
   *
   * Allows compiler optimizations on access.
   *
   * @param position the position to query.
   * @return the character at said position.
   */
  inline char getCharAt(unsigned long position);
  /**
   * Inline Alias to detect characters not used in keywords.
   *
   * Uses int range comparisons for performance.
   * @param position the position to query.
   * @return true if the character ends a keyword.
   */
  inline bool keyWordEnd(unsigned long position);
  /**
   * Utility method to abort with a lexer error.
   * @return returns an invalid token.
   */
  inline Token failParsing();
  /**
   * Munches all whitespace in the content that it can find.
   * @return one whitespace token, regardless of actual amount
   */
  inline Token munchWhitespace();
  /**
   * Munches till the end of the line, ignoring content
   * @return one linecomment token
   */
  inline Token munchLineComment();
  /**
   * Munches till a terminating block comment token is found
   * @return one blockcomment token
   */
  inline Token munchBlockComment();
  /**
   * Munches one integer number as greedy as possible
   * @return one number token containing the integer as a string
   */
  inline Token munchNumber();
  /**
   * Munches one identifier as greedy as possible.
   *
   * This method assumes that we already know that
   * this is no keyword.
   * @return one identifier token containing the identifier
   */
  inline Token munchIdentifier();
  /**
   * Munch one character definition with the quotes.
   * @return one character token with the quoted content
   */
  inline Token munchCharacter();
  /**
   * Munch one string definition with the quotes
   * @return one string token with the quoted content
   */
  inline Token munchString();
  /**
   * Check if the content matches a known keyword
   * and munches it. Returns nonkeyword if not.
   *
   * This method is optimized with a jump table for performance.
   * @return one keyword token or nonkeyword
   */
  inline Token munchKeyword();
  /**
   * Check if the content matches a known punctuator
   * and munches it. Returns invalid if not.
   *
   * This method is optimized with a jump table for performance.
   * @return one keyword token or invalid
   */
  inline Token munchPunctuator();

public:
  /**
   * Initialize a FastLexer
   * @param content the content to be lexed
   * @param f a filename used for output prefix
   */
  explicit FastLexer(const std::string &content, std::string f = "")
      : filename(std::move(f)), content(content) {}
  /**
   * Lex the content
   * @return A vector of lexed tokens
   */
  std::vector<ccc::Token, std::allocator<ccc::Token>> lex();
  /**
   * Lex the content till the next valid Token
   * @return the next valid Token
   */
  Token lex_valid();
  /**
   * Checks for errors during lexing
   * @return true if errors happened
   */
  bool fail() const { return !error.empty(); }
  /**
   * Retrieves errors in a human readable format
   * @return the presentable error message
   */
  const std::string getError() const {
    return (filename.empty() ? filename : filename + ":") + error;
  }
  /**
   * Let the content and print out tokens to std::cout.
   *
   * Separated for performance reasons.
   */
  void tokenize();
};

} // namespace ccc

#endif // C4_FASTLEXER_H
