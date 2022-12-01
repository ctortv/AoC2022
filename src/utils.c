#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

#include "../include/utils.h"

unsigned long parse_arg(const char * arg, bool exit_on_fail) {
  static const int AOC_BASE = 10;
  char * end = NULL;

  const unsigned long number = strtoul(arg, &end, AOC_BASE);

  if(arg == end) { goto err0; }
  if(errno == ERANGE) { goto err1; }
  if(errno) { goto err2; }

  return number;

err2:
  fprintf(stderr, "you're a bad friend, what is this? it should be a number: %s.\n", arg);
  goto err;

err1:
  fprintf(stderr, "you're outta range, bud. this number is too big and/or small for me: %s.\n", arg);
  goto err;

err0:
  fprintf(stderr, "what even are you doing? '%s' is not a number i recognize.\n", arg);
  goto err;

err:
  if(exit_on_fail) {
    exit(-1);
  } else {
    return 0;
  }
}


