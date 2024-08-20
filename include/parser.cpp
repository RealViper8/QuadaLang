#include "parser.h"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <variant>

token_T seek_token(parser_T *parser, size_t offset) {
  return parser->tokens.at(parser->current + offset);
}

void link_tokens(parser_T *parser) {

  size_t currIndex = 1;
  for (token_T token : parser->tokens) {
    switch (token.type) {
    case TOKEN_PUSH:
      if (parser->tokens.size() > currIndex) {
        parser->tokens.insert(
            parser->tokens.begin() + currIndex,
            init_token((parser->tokens.at(currIndex).type == TOKEN_INT)
                           ? TOKEN_INT
                           : TOKEN_PUSH,
                       parser->tokens.at(currIndex).value, false));
        parser->tokens.erase(parser->tokens.begin() + currIndex - 1);
        parser->tokens.erase(parser->tokens.begin() + currIndex);
      }
      currIndex++;
      break;
    case TOKEN_SET:
      if (parser->tokens.size() > currIndex) {
        parser->tokens.insert(
            parser->tokens.begin() + currIndex,
            init_token(TOKEN_SET, parser->tokens.at(currIndex).value, false,
                       parser->tokens.at(currIndex + 1).value));
        parser->tokens.erase(parser->tokens.begin() + currIndex - 1);
        parser->tokens.erase(parser->tokens.begin() + currIndex);
      }
      currIndex++;
    case TOKEN_PRINT:
      currIndex++;
      break;
    case TOKEN_EXIT:
      currIndex++;
    default:
      break;
    }
  }
}

void print_token(token_T token) {
  switch (token.type) {
  case TOKEN_PRINT:
    std::cout << "Print: " << token.value << std::endl;
    break;
  case TOKEN_INT:
    std::cout << "Int: " << token.value << std::endl;
    break;
  case TOKEN_JMP:
    std::cout << "Jmp: " << token.value << std::endl;
    break;
  case TOKEN_STRING:
    std::cout << "String: " << token.value << std::endl;
    break;
  case TOKEN_NEQ:
    std::cout << "IFNEQ: " << token.value << std::endl;
    break;
  case TOKEN_PUSH:
    std::cout << "PUSH: " << token.value << std::endl;
    break;
  case TOKEN_IFEQ:
    std::cout << "IFEQ: " << token.value << std::endl;
    break;
  case TOKEN_IMPORT:
    std::cout << "IMPORT: " << token.value << std::endl;
    break;
  case TOKEN_EXIT:
    std::cout << "EXIT: " << token.value << std::endl;
    break;
  case TOKEN_SET:
    std::cout << "SET: " << token.value << std::endl;
    break;
  default:
    std::cout << "TOKEN_TYPE: " << token.type << "\nValue: " << token.value
              << std::endl;
    break;
  }
}

Error parse_tokens(parser_T *parser) {
  link_tokens(parser);

  size_t i = 0;
  for (token_T token : parser->tokens) {
    // print_token(token);
    switch (token.type) {
    case TOKEN_PUSH:
      parser->stack.push(token.value);
      i++;
      break;
    case TOKEN_PRINT:
      while (!parser->stack.empty()) {
        std::visit([](const auto &arg) { std::cout << arg << std::endl; },
                   parser->stack.top());
        parser->stack.pop();
        i++;
      }
      break;
    case TOKEN_EXIT:
      if (!parser->stack.empty() &&
          std::holds_alternative<int>(parser->stack.top())) {
        std::exit(std::get<int>(parser->stack.top()));
        parser->stack.pop();
        i++;
      } else {
        return ERROR_PREP(ERROR_EXPECTED_INT, "Expected int got string !",
                          i + parser->newlines);
      }
      break;
    case TOKEN_INT:
      parser->stack.push(std::stoi(token.value));
      i++;
      break;
    case TOKEN_IMPORT:
      parser->stack.push(token.value);
      i++;
      break;
    case TOKEN_SET:
      parser->variables.emplace(token.name, token.value);
      i++;
      break;
    default:
      break;
    }
  }

  return ERROR_PREP(ERROR_NONE, "", 0);
}

int tokenize(parser_T *parser) {
  std::vector<token_T> tokens;

  std::string str;
  std::string num;

  std::string token;
  std::string name;

  bool isString = false;
  bool comment = false;

  size_t index = 0;
  parser->tokens.resize(parser->chunks.size() / 10);
chunk_detect:
  for (char c : parser->chunks[parser->current - 1]) {

    if (token == "print") {
      parser->tokens.push_back(init_token(TOKEN_PRINT, token, true));
      token.clear();
    } else if (token == "exit") {
      parser->tokens.push_back(init_token(TOKEN_EXIT, token, true));
      token.clear();
    } else if (token == "push") {
      parser->tokens.push_back(init_token(TOKEN_PUSH, token, false));
      token.clear();
    } else if (token == "import") {
      parser->tokens.push_back(init_token(TOKEN_IMPORT, token, false));
      token.clear();
    } else if (token == "set") {
      parser->tokens.push_back(init_token(TOKEN_SET, token, false));
      token.clear();
    } else if (token == "use") {
      parser->tokens.push_back(init_token(TOKEN_USE, token, false));
      token.clear();
    } else if (token == "exit") {
      parser->tokens.push_back(init_token(TOKEN_EXIT, token, false));
      token.clear();
    } else if (c == ';') {
      comment = true;
    } else if (c == '(') {
      parser->tokens.push_back(init_token(TOKEN_LPARREN, "(", false));
    } else if (c == ')') {
      parser->tokens.push_back(init_token(TOKEN_RPARREN, ")", false));
    } else if ((c == '"' || c == '\'') && isString == false) {
      isString = true;
    } else if (isString && c != '"' && c != '\'') {
      str += c;
    } else if (isString && (c == '"' || c == '\'')) {
      parser->tokens.push_back(init_token(TOKEN_STRING, str, false));
      isString = false;
      str.clear();
      continue;
    } else if (!isString && isdigit(c)) {
      num += c;
    }

    index++;
    if (comment == true && c != '\n')
      continue;
    else if (comment == true && c == '\n') {
      parser->newlines++;
      comment = false;
    }

    if (c == '\0' || c == '\n') {
      if ((parser->chunks[parser->current - 1][index] == '\n' ||
           parser->chunks[parser->current - 1][index - 1] == '\n') &&
          parser->chunks[parser->current - 1][index + 1] == '\n')
        parser->newlines++;
      continue;
    }

    if (!num.empty() && parser->chunks[parser->current - 1][index] == '\n') {
      parser->tokens.push_back(init_token(TOKEN_INT, num, false));
      num.clear();
      continue;
    } else if (!num.empty() &&
               isdigit(parser->chunks[parser->current - 1][index]))
      continue;

    if (!isString && !isspace(c)) {
      token += c;
    }

    // std::cout << str << std::endl;
  }

  if (parser->chunks.size() == parser->current) {
    Error res = parse_tokens(parser);
    if (res.type != ERROR_NONE) {
      print_error(res);
      return 1;
    }
  } else {
    parser->current++;
    goto chunk_detect;
  }

  return 0;
}

int parse(parser_T *parser) {
  std::ifstream file(parser->file);

  if (!file)
    return 1;

  while (!file.eof()) {
    std::vector<char> chunk(parser->chunkSize);
    file.read(chunk.data(), parser->chunkSize);
    std::size_t bytesRead = file.gcount();

    if (bytesRead > 0) {
      chunk.resize(bytesRead);
      parser->chunks.push_back(std::move(chunk));
    }

    for (int i = 0; i < parser->chunks.size(); i++) {
      std::cout << "Chunks: \n";
      for (char c : parser->chunks[i]) {
        std::cout << c;
      }
      std::cout << "----------------\n";
    }
  }

  file.close();

  int res = tokenize(parser);
  if (res > 0) {
    return 1;
  }

  return 0;
}

token_T init_token(const Token type, const std::string value, bool interrupt,
                   std::string name) {
  token_T token;
  token.type = type;
  token.value = value;
  token.interrupt = interrupt;
  token.name = name;
  return token;
}

parser_T init_parser(const std::string filename) {
  parser_T parser;
  parser.file = filename;
  return parser;
}
