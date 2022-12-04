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


int read_lines(int argc, char **argv, void * state, aoc_fn fn) {
  if(argc < 4) { goto err0; }

  const char * input = argv[3];

  FILE * fp = fopen(input, "r");
  if(!fp || errno) { goto err1; }

  char * line = NULL;
  size_t len = 0;
  ssize_t read = 0;

  while((read = getline(&line, &len, fp)) != -1) {
    if(errno) { goto err2; }
    fn(line, read, state);
  }

  free(line), line = NULL;
  fclose(fp);

  return EXIT_SUCCESS;

err2:
  fclose(fp);
  free(line), line = NULL;
  fprintf(stderr, "oh no, i can't read a line from '%s'! i'm dead. :(\n", input);
  goto err;

err1:
  fprintf(stderr, "'%s' doesn't exist or i couldn't open it. i tried :(\n", input);
  goto err;

err0:
  fprintf(stderr, "listen, i need a path/file name, bud.\n");
  fprintf(stderr, "usage: aoc 1 0 <path>\n");
  goto err;

err:
  return EXIT_FAILURE;
}



