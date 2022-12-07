#ifndef AOC_UTILS__H
#define AOC_UTILS__H

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

typedef void(*aoc_fn)(const char * line, ssize_t read, void * state);

unsigned long parse_arg(const char *, bool);
void parse_uint(const char * str, uint32_t * I);
uint32_t read_uint(const char ** str);
int read_lines(int argc, char **argv, void * state, aoc_fn fn) ;

inline void swapul(unsigned long *lhs, unsigned long *rhs) {
  unsigned long temp = *rhs;
  *rhs = *lhs;
  *lhs = temp;
}

#endif

