#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <sys/types.h>

#define ERROR_PREP(t, m, l)                                                    \
  Error { .type = t, .msg = m, .line = l }

#define ERROR_CHANGE(n, t, m, l)                                               \
  n.type = t;                                                                  \
  n.msg = m;                                                                   \
  n.line = l;

typedef enum : uint {
  ERROR_NONE = 0,
  ERROR_EXPECTED_STRING,
  ERROR_EXPECTED_INT,
  ERROR_INCORRECT_FUNCTION,
  ERROR_SYNTAX,
} error_T;

typedef struct ERROR_STRUCT {
  error_T type;
  std::string msg;

  size_t line;
} Error;

void print_error(Error);

#endif // !TYPES_H
