#ifndef AOC_UTILS__H
#define AOC_UTILS__H

#include <stdint.h>
#include <stdbool.h>

unsigned long parse_arg(const char *, bool);

inline void swapul(unsigned long *lhs, unsigned long *rhs) {
  unsigned long temp = *rhs;
  *rhs = *lhs;
  *lhs = temp;
}

#endif

