#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/utils.h"
#include "../include/day4.h"

static void aoc_day5_p0_worker(const char * line, ssize_t read, void * state) {
  (void)state;
  if(read <= 1) { return; }
  size_t len = strnlen(line, 65536);
  const char * s = line;
  const char * end = line + len;
  while(s < end) {
    char T[5] = { s[0], s[1], s[2], s[3], '\0' };
    if(T[0] == T[1]) { goto next; }
    if(T[0] == T[2]) { goto next; }
    if(T[0] == T[3]) { goto next; }
    if(T[1] == T[2]) { goto next; }
    if(T[1] == T[3]) { goto next; }
    if(T[2] == T[3]) { goto next; }
    break;

next:
    s++;
  }

  fprintf(stdout, "Characters before SoP Marker: %zu\n", (s - line) + 4);
}

int aoc_day5_p0(int argc, char **argv) {
  int result = read_lines(argc, argv, NULL, &aoc_day5_p0_worker);
  return result;
}

static void aoc_day5_p1_worker(const char * line, ssize_t read, void * state) {
  (void)state;
  if(read <= 1) { return; }
  size_t len = strnlen(line, 65536);
  const char * s = line;
  const char * end = line + len;
  while(s < end) {
    char T[15] = { 0 };
    memcpy(T, s, sizeof T);

    for(size_t i = 0; i < (sizeof T) - 2; i++) {
      for(size_t j = i + 1; j < (sizeof T) - 1; j++) {
        if(T[i] == T[j]) { goto next; }
      }
    }

    break;

next:
    s++;
  }

  fprintf(stdout, "Characters before Message Marker: %zu\n", (s - line) + 14);
}

int aoc_day5_p1(int argc, char **argv) {
  int result = read_lines(argc, argv, NULL, &aoc_day5_p1_worker);
  return result;
}


