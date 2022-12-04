#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../include/utils.h"
#include "../include/day3.h"

typedef struct {
  uint32_t total_score;
} day_3_state;

// 2-4
typedef struct {
  uint32_t min;
  uint32_t max;
} range;

// 2-4,6-8
typedef struct {
  range left;
  range right;
} pair;

static void parse_int(const char * str, uint32_t * I);
static void parse_range(const char ** str, range * R);
static void parse_pair(const char ** str, pair * P);
static inline void consume(const char **s) { (*s)++; }

/*
--- Day 4: Camp Cleanup ---

Space needs to be cleared before the last supplies can be unloaded from the
ships, and so several Elves have been assigned the job of cleaning up sections
of the camp. Every section has a unique ID number, and each Elf is assigned a
range of section IDs.

However, as some of the Elves compare their section assignments with each
other, they've noticed that many of the assignments overlap. To try to quickly
find overlaps and reduce duplicated effort, the Elves pair up and make a big
list of the section assignments for each pair (your puzzle input).

For example, consider the following list of section assignment pairs:

2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8

For the first few pairs, this list means:

    Within the first pair of Elves, the first Elf was assigned sections 2-4
    (sections 2, 3, and 4), while the second Elf was assigned sections 6-8
    (sections 6, 7, 8).

    The Elves in the second pair were each assigned two sections.

    The Elves in the third pair were each assigned three sections: one got
    sections 5, 6, and 7, while the other also got 7, plus 8 and 9.

This example list uses single-digit section IDs to make it easier to draw; your
actual list might contain larger numbers. Visually, these pairs of section
assignments look like this:

.234.....  2-4
.....678.  6-8

.23......  2-3
...45....  4-5

....567..  5-7
......789  7-9

.2345678.  2-8
..34567..  3-7

.....6...  6-6
...456...  4-6

.23456...  2-6
...45678.  4-8

Some of the pairs have noticed that one of their assignments fully contains the
other. For example, 2-8 fully contains 3-7, and 6-6 is fully contained by 4-6.
In pairs where one assignment fully contains the other, one Elf in the pair
would be exclusively cleaning sections their partner will already be cleaning,
so these seem like the most in need of reconsideration. In this example, there
are 2 such pairs.

In how many assignment pairs does one range fully contain the other?

*/

static void aoc_day3_p0_worker(const char * line, ssize_t read, void * state) {
  (void)read;
  day_3_state * S = (day_3_state *)state;

  const char * s = line;
  pair P = { 0 };
  parse_pair(&s, &P);
  const range L = P.left, R = P.right;
  if((L.min >= R.min && L.max <= R.max) || (R.min >= L.min && R.max <= L.max)) {
    S->total_score++;
  }
}

int aoc_day3_p0(int argc, char **argv) {
  day_3_state state = { 0 };
  int result = read_lines(argc, argv, &state, &aoc_day3_p0_worker);
  fprintf(stdout, "Assignment pairs inclusive: %i\n", state.total_score);
  return result;
}


/*
--- Part Two ---

It seems like there is still quite a bit of duplicate work planned. Instead,
the Elves would like to know the number of pairs that overlap at all.

In the above example, the first two pairs (2-4,6-8 and 2-3,4-5) don't overlap,
while the remaining four pairs (5-7,7-9, 2-8,3-7, 6-6,4-6, and 2-6,4-8) do
overlap:

    5-7,7-9 overlaps in a single section, 7.
    2-8,3-7 overlaps all of the sections 3 through 7.
    6-6,4-6 overlaps in a single section, 6.
    2-6,4-8 overlaps in sections 4, 5, and 6.

So, in this example, the number of overlapping assignment pairs is 4.

In how many assignment pairs do the ranges overlap?
*/

static void aoc_day3_p1_worker(const char * line, ssize_t read, void * state) {
  (void)read;
  day_3_state * S = (day_3_state *)state;
  const char * s = line;

  pair P = { 0 };
  parse_pair(&s, &P);
  const range L = P.left, R = P.right;
  if((L.min <= R.max && L.max >= R.max) || (R.min <= L.max && R.max >= L.max)) {
    S->total_score++;
  }
}

int aoc_day3_p1(int argc, char **argv) {
  day_3_state state = { 0 };
  int result = read_lines(argc, argv, &state, &aoc_day3_p1_worker);
  fprintf(stdout, "Assignment pairs inclusive: %i\n", state.total_score);
  return result;
}

static void parse_pair(const char ** str, pair * P) {
  parse_range(str, &(P->left));
  parse_range(str, &(P->right));
}

static void parse_range(const char ** str, range * R) {
  #define max_num_len 64
  assert(str && *str);
  const char * s = *str, * first = *str;
  while(s && *s && *s != '\0') {
    bool newline = *s == '\n'
      // winderz (untested):
      || (*s == '\r' && *(s + 1) == '\n')
      ;
    const char prev = *s;
    if(*s == '-' || (*s == ',' || newline)) {
      if(*s == '-' || *s == ',') { consume(&s); }
      if(newline) {
        if(*s == '\r') { consume(&s); }
        consume(&s);
      }
      char temp_buffer[max_num_len] = { 0 };
      memmove(temp_buffer, first, s - first - 1);
      uint32_t * dest = (prev == ',' || newline) ? &(R->max) : &(R->min);
      first = s;
      parse_int(temp_buffer, dest);
    }

    if(newline || prev == ',') {
      break;
    }
    consume(&s);
  }

  *str = s;
  #undef max_num_len
}

static void parse_int(const char * str, uint32_t * I) {
  static const int AOC_BASE = 10;
  const unsigned long number = strtoul(str, NULL, AOC_BASE);
  if(errno == ERANGE) { goto err; }
  if(errno) { goto err; }
  assert(number <= UINT32_MAX);
  *I = (uint32_t)number;

  return;

err:
  fprintf(stderr, "parse error: %s cannot parse to unsigned long.\n", str);
  exit(EXIT_FAILURE);
}

