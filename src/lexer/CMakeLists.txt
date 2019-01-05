SET(lexer_SRCS fast_lexer.cpp token.cpp)

add_library(lexer SHARED ${lexer_SRCS})
