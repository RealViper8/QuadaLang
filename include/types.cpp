#include "types.h"
#include <iostream>

void print_error(Error err) {
  if (err.type == ERROR_NONE)
    return;
  switch (err.type) {
  case ERROR_EXPECTED_INT:
    std::cout << "Error: EXPECTED INT" << std::endl;
    break;
  case ERROR_SYNTAX:
    std::cout << "Error: SYNTAX\n";
    break;
  case ERROR_EXPECTED_STRING:
    std::cout << "Error: EXPECTED STRING" << std::endl;
  case ERROR_NONE:
    break;
  case ERROR_INCORRECT_FUNCTION:
    std::cout << "Inavlid function name: " << err.msg << std::endl;
    break;
  }

  std::cout << "    -> " << err.msg << std::endl;
  std::cout << " line: " << err.line << std::endl;
}
