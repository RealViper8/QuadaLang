#ifndef PARSER_H
#define PARSER_H

#include "types.h"

#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

typedef enum : uint {
  TOKEN_PRINT,
  TOKEN_USE,
  TOKEN_SET,
  TOKEN_TO,
  TOKEN_JMP,
  TOKEN_PUSH,
  TOKEN_IFEQ,
  TOKEN_IMPORT,
  TOKEN_STRING,
  TOKEN_LPARREN,
  TOKEN_RBRACE,
  TOKEN_RPARREN,
  TOKEN_LBRACE,
  TOKEN_INT,
  TOKEN_NEQ,
  TOKEN_ID,
  TOKEN_EXIT,
} Token;

typedef struct TOKEN_STRUCT {
  Token type;
  bool interrupt = false;
  std::string value;
  std::string name;
} token_T;

typedef struct PARSER_STRUCT {
  std::vector<token_T> tokens;
  std::unordered_map<std::string, std::variant<int, std::string>> variables;
  std::stack<std::variant<int, std::string>> stack;
  std::string file;

  size_t current = 1;
  size_t newlines = 0;
  bool debug = false;

  const std::size_t chunkSize = 1024;
  std::vector<std::vector<char>> chunks;
} parser_T;

parser_T init_parser(const std::string);
token_T init_token(const Token type, const std::string value, bool,
                   std::string name = "");

int tokenize(parser_T *);
Error parse_tokens(parser_T *);
void print_token(token_T token);
void link_tokens(parser_T *);

token_T seek_token(parser_T *);
int parse(parser_T *);

#endif // !PARSER_H
