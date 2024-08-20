#include "../include/parser.h"
#include "../include/standard.h"
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: quada <file.quada>\n";
    return 1;
  }

  if (!std::filesystem::exists(argv[1])) {
    std::cout << "Path: " << argv[1] << " not found\n";
    return 1;
  }

  parser_T parser = init_parser(argv[1]);

  parse(&parser);

  return 0;
}
