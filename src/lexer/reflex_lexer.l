%top{
  #include <cstdio>
  #include <iostream>
  #include <iomanip>
  #include "token.hpp"
  using namespace std;
%}

%class{
  std::list<Token, std::allocator<Token>> token_list;
 public:
  std::list<Token, std::allocator<Token>> results() {
    return token_list;
  }
%}

%init{
  token_list = std::list<Token>();
%}


%option fast

WHITESPACE	[[:space:]]+
ILCOMMENT	"//"([^\n]|\\\n)*\n
MLCOMMENT	"/*"(.|\n)*?"*/"
NAME		[[:alpha:]_][[:alnum:]_]*
digit       [0-9]
digit1      [1-9]
digits      {digit}+
int         -?{digit}|-?{digit1}{digits}
CHAR		L?'(\\.|[^\\\n'])*'
STRING		L?\"(\\.|\\\r?\n|[^\\\n"])*\"



%%

{WHITESPACE}
{ILCOMMENT}
{MLCOMMENT}
"auto"	token_list.emplace_back(Token(TokenType::AUTO, lineno(), columno(), str()));
"break"	token_list.emplace_back(Token(TokenType::BREAK, lineno(), columno(), str()));
"case"	token_list.emplace_back(Token(TokenType::CASE, lineno(), columno(), str()));
"char"	token_list.emplace_back(Token(TokenType::CHAR, lineno(), columno(), str()));
"const"	token_list.emplace_back(Token(TokenType::CONST, lineno(), columno(), str()));
"continue"	token_list.emplace_back(Token(TokenType::CONTINUE, lineno(), columno(), str()));
"default"	token_list.emplace_back(Token(TokenType::DEFAULT, lineno(), columno(), str()));
"do"	token_list.emplace_back(Token(TokenType::DO, lineno(), columno(), str()));
"else"	token_list.emplace_back(Token(TokenType::ELSE, lineno(), columno(), str()));
"enum"	token_list.emplace_back(Token(TokenType::ENUM, lineno(), columno(), str()));
"extern"	token_list.emplace_back(Token(TokenType::EXTERN, lineno(), columno(), str()));
"for"	token_list.emplace_back(Token(TokenType::FOR, lineno(), columno(), str()));
"goto"	token_list.emplace_back(Token(TokenType::GOTO, lineno(), columno(), str()));
"if"	token_list.emplace_back(Token(TokenType::IF, lineno(), columno(), str()));
"inline"	token_list.emplace_back(Token(TokenType::INLINE, lineno(), columno(), str()));
"int"	token_list.emplace_back(Token(TokenType::INT, lineno(), columno(), str()));
"long"	token_list.emplace_back(Token(TokenType::LONG, lineno(), columno(), str()));
"register"	token_list.emplace_back(Token(TokenType::REGISTER, lineno(), columno(), str()));
"restrict"	token_list.emplace_back(Token(TokenType::RESTRICT, lineno(), columno(), str()));
"return"	token_list.emplace_back(Token(TokenType::RETURN, lineno(), columno(), str()));
"short"	token_list.emplace_back(Token(TokenType::SHORT, lineno(), columno(), str()));
"signed"	token_list.emplace_back(Token(TokenType::SIGNED, lineno(), columno(), str()));
"sizeof"	token_list.emplace_back(Token(TokenType::SIZEOF, lineno(), columno(), str()));
"static"	token_list.emplace_back(Token(TokenType::STATIC, lineno(), columno(), str()));
"struct"	token_list.emplace_back(Token(TokenType::STRUCT, lineno(), columno(), str()));
"switch"	token_list.emplace_back(Token(TokenType::SWITCH, lineno(), columno(), str()));
"typedef"	token_list.emplace_back(Token(TokenType::TYPEDEF, lineno(), columno(), str()));
"union"	token_list.emplace_back(Token(TokenType::UNION, lineno(), columno(), str()));
"unsigned"	token_list.emplace_back(Token(TokenType::UNSIGNED, lineno(), columno(), str()));
"void"	token_list.emplace_back(Token(TokenType::VOID, lineno(), columno(), str()));
"volatile"	token_list.emplace_back(Token(TokenType::VOLATILE, lineno(), columno(), str()));
"while"	token_list.emplace_back(Token(TokenType::WHILE, lineno(), columno(), str()));
"_Alignas"	token_list.emplace_back(Token(TokenType::ALIGN_AS, lineno(), columno(), str()));
"_Alignof"	token_list.emplace_back(Token(TokenType::ALIGN_OF, lineno(), columno(), str()));
"_Atomic"	token_list.emplace_back(Token(TokenType::ATOMIC, lineno(), columno(), str()));
"_Bool"	token_list.emplace_back(Token(TokenType::BOOL, lineno(), columno(), str()));
"_Complex"	token_list.emplace_back(Token(TokenType::COMPLEX, lineno(), columno(), str()));
"_Generic"	token_list.emplace_back(Token(TokenType::GENERIC, lineno(), columno(), str()));
"_Imaginary"	token_list.emplace_back(Token(TokenType::IMAGINARY, lineno(), columno(), str()));
"_Noreturn"	token_list.emplace_back(Token(TokenType::NO_RETURN, lineno(), columno(), str()));
"_Static_assert"	token_list.emplace_back(Token(TokenType::STATIC_ASSERT, lineno(), columno(), str()));
"_Thread_local"	token_list.emplace_back(Token(TokenType::THREAD_LOCAL, lineno(), columno(), str()));

{NAME}		token_list.emplace_back(Token(TokenType::IDENTIFIER, lineno(), columno(), str()));
{int}		token_list.emplace_back(Token(TokenType::NUMBER, lineno(), columno(), str()));
{CHAR}		token_list.emplace_back(Token(TokenType::CHAR, lineno(), columno(), str()));
{STRING}	token_list.emplace_back(Token(TokenType::STRING, lineno(), columno(), str()));

"{"|"<%"	token_list.emplace_back(Token(TokenType::BRACE_OPEN, lineno(), columno(), str()));
"}"|"%>"	token_list.emplace_back(Token(TokenType::BRACE_CLOSE, lineno(), columno(), str()));
"["|"<:"	token_list.emplace_back(Token(TokenType::BRACKET_OPEN, lineno(), columno(), str()));
"]"|":>"	token_list.emplace_back(Token(TokenType::BRACKET_CLOSE, lineno(), columno(), str()));
"("		    token_list.emplace_back(Token(TokenType::PARENTHESIS_OPEN, lineno(), columno(), str()));
")"		    token_list.emplace_back(Token(TokenType::PARENTHESIS_CLOSE, lineno(), columno(), str()));
"+="		token_list.emplace_back(Token(TokenType::PLUS_ASSIGN, lineno(), columno(), str()));
"++"		token_list.emplace_back(Token(TokenType::PLUSPLUS, lineno(), columno(), str()));
"+"		    token_list.emplace_back(Token(TokenType::PLUS, lineno(), columno(), str()));
"-="		token_list.emplace_back(Token(TokenType::MINUS_ASSIGN, lineno(), columno(), str()));
"--"		token_list.emplace_back(Token(TokenType::MINUSMINUS, lineno(), columno(), str()));
"->*"		token_list.emplace_back(Token(TokenType::ARROW_STAR, lineno(), columno(), str()));
"->"		token_list.emplace_back(Token(TokenType::ARROW, lineno(), columno(), str()));
"-"		    token_list.emplace_back(Token(TokenType::MINUS, lineno(), columno(), str()));
"=="		token_list.emplace_back(Token(TokenType::EQUAL, lineno(), columno(), str()));
"="		    token_list.emplace_back(Token(TokenType::ASSIGN, lineno(), columno(), str()));
"<="		token_list.emplace_back(Token(TokenType::LESS_EQUAL, lineno(), columno(), str()));
"<<="		token_list.emplace_back(Token(TokenType::LEFT_SHIFT_ASSIGN, lineno(), columno(), str()));
"<<"		token_list.emplace_back(Token(TokenType::LEFT_SHIFT, lineno(), columno(), str()));
"<"		    token_list.emplace_back(Token(TokenType::LEFT, lineno(), columno(), str()));
">="		token_list.emplace_back(Token(TokenType::GREATER_EQUAL, lineno(), columno(), str()));
">>="		token_list.emplace_back(Token(TokenType::RIGHT_SHIFT_ASSIGN, lineno(), columno(), str()));
">>"		token_list.emplace_back(Token(TokenType::RIGHT_SHIFT, lineno(), columno(), str()));
">"		    token_list.emplace_back(Token(TokenType::RIGHT, lineno(), columno(), str()));
"!="		token_list.emplace_back(Token(TokenType::NOT_EQUAL, lineno(), columno(), str()));
"!"		    token_list.emplace_back(Token(TokenType::NOT, lineno(), columno(), str()));
","		    token_list.emplace_back(Token(TokenType::COMMA, lineno(), columno(), str()));
";"		    token_list.emplace_back(Token(TokenType::SEMICOLON, lineno(), columno(), str()));
"..."		token_list.emplace_back(Token(TokenType::TRI_DOTS, lineno(), columno(), str()));
".*"		token_list.emplace_back(Token(TokenType::DOT_STAR, lineno(), columno(), str()));
"."		    token_list.emplace_back(Token(TokenType::DOT, lineno(), columno(), str()));
"^="		token_list.emplace_back(Token(TokenType::CARET_ASSIGN, lineno(), columno(), str()));
"^"		    token_list.emplace_back(Token(TokenType::CARET, lineno(), columno(), str()));
"~"		    token_list.emplace_back(Token(TokenType::TILDE, lineno(), columno(), str()));
"*="		token_list.emplace_back(Token(TokenType::STAR_ASSIGN, lineno(), columno(), str()));
"*"		    token_list.emplace_back(Token(TokenType::STAR, lineno(), columno(), str()));
"/="		token_list.emplace_back(Token(TokenType::DIV_ASSIGN, lineno(), columno(), str()));
"/"		    token_list.emplace_back(Token(TokenType::DIV, lineno(), columno(), str()));
"%="		token_list.emplace_back(Token(TokenType::MOD_ASSIGN, lineno(), columno(), str()));
"%"		    token_list.emplace_back(Token(TokenType::MOD, lineno(), columno(), str()));
"&="		token_list.emplace_back(Token(TokenType::AMPERSAND_ASSIGN, lineno(), columno(), str()));
"&&"		token_list.emplace_back(Token(TokenType::AND, lineno(), columno(), str()));
"&"		    token_list.emplace_back(Token(TokenType::AMPERSAND, lineno(), columno(), str()));
"|="		token_list.emplace_back(Token(TokenType::PIPE_ASSIGN, lineno(), columno(), str()));
"||"		token_list.emplace_back(Token(TokenType::OR, lineno(), columno(), str()));
"|"		    token_list.emplace_back(Token(TokenType::PIPE, lineno(), columno(), str()));
"::"		token_list.emplace_back(Token(TokenType::COLON_COLON, lineno(), columno(), str()));
":"		    token_list.emplace_back(Token(TokenType::COLON, lineno(), columno(), str()));
"?"		    token_list.emplace_back(Token(TokenType::QUESTION, lineno(), columno(), str()));
.		printf("*** ERROR '%s' at line %zu\n", text(), lineno());

%%

