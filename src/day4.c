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

/* worst code i've ever written - @ctor@mastodon.online */

/*
--- Day 5: Supply Stacks ---

The expedition can depart as soon as the final supplies have been unloaded from
the ships. Supplies are stored in stacks of marked crates, but because the
needed supplies are buried under many other crates, the crates need to be
rearranged.

The ship has a giant cargo crane capable of moving crates between stacks. To
ensure none of the crates get crushed or fall over, the crane operator will
rearrange them in a series of carefully-planned steps. After the crates are
rearranged, the desired crates will be at the top of each stack.

The Elves don't want to interrupt the crane operator during this delicate
procedure, but they forgot to ask her which crate will end up where, and they
want to be ready to unload them as soon as possible so they can embark.

They do, however, have a drawing of the starting stacks of crates and the
rearrangement procedure (your puzzle input). For example:

    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2

In this example, there are three stacks of crates. Stack 1 contains two crates:
crate Z is on the bottom, and crate N is on top. Stack 2 contains three crates;
from bottom to top, they are crates M, C, and D. Finally, stack 3 contains a
single crate, P.

Then, the rearrangement procedure is given. In each step of the procedure, a
quantity of crates is moved from one stack to a different stack. In the first
step of the above rearrangement procedure, one crate is moved from stack 2 to
stack 1, resulting in this configuration:

[D]
[N] [C]
[Z] [M] [P]
 1   2   3

In the second step, three crates are moved from stack 1 to stack 3. Crates are
moved one at a time, so the first crate to be moved (D) ends up below the
second and third crates:

        [Z]
        [N]
    [C] [D]
    [M] [P]
 1   2   3

Then, both crates are moved from stack 2 to stack 1. Again, because crates are
moved one at a time, crate C ends up below crate M:

        [Z]
        [N]
[M]     [D]
[C]     [P]
 1   2   3

Finally, one crate is moved from stack 1 to stack 2:

        [Z]
        [N]
        [D]
[C] [M] [P]
 1   2   3

The Elves just need to know which crate will end up on top of each stack; in
this example, the top crates are C in stack 1, M in stack 2, and Z in stack 3,
so you should combine these together and give the Elves the message CMZ.

After the rearrangement procedure completes, what crate ends up on top of each
stack?
*/

typedef struct {
  uint32_t count;
  uint32_t src;
  uint32_t dest;
} procedure;

static const char EMPTY = '\0';

#define MAX_ROWS 64
#define MAX_COLS 64

static size_t current = 0;
static size_t columns_len = 0;

static size_t row_len = 32;
static char crates[MAX_ROWS][MAX_COLS] = { 0 };

typedef struct {
  const char ** str;
  size_t consumed;
  char * message;
  size_t procs_sz;
  size_t procs_len;
  procedure * procs;
} day_4_state;

static uint32_t read_uint(day_4_state * S) {
  char buf[/* arbitrary */ 64] = { 0 };
  const char * s = *(S->str);
  char * b = &buf[0];
  while(s && *s != '\0') {
    if(isspace(*s)) { break; }
    if(*s >= '0' && *s <= '9') {
      *b = *s;
      b++;
    }
    s++;
  }

  uint32_t value = 0;
  parse_uint(buf, &value);
  *(S->str) = s;
  return value;
}

static void consume_space(day_4_state * S) {
  while(isspace(**(S->str))) { S->consumed++; (*S->str)++; }
}

static void consume_str(const char * match, day_4_state * S) {
  S->consumed = strnlen(match, 32);
  if(strncmp(match, *(S->str), S->consumed) == 0) {
    *S->str += S->consumed;
  } else {
    fprintf(stderr, "parse error at '%s'.\n", match);
    exit(EXIT_FAILURE);
  }
}

static uint32_t consume_move(day_4_state * S) {
  consume_str("move ", S);
  return read_uint(S);
}

static uint32_t consume_from(day_4_state * S) {
  consume_str("from ", S);
  return read_uint(S);
}

static uint32_t consume_to(day_4_state * S) {
  consume_str("to ", S);
  return read_uint(S);
}

static void resize_instructions(day_4_state *S) {
  size_t new_size = S->procs_sz * sizeof * S->procs;
  if(new_size > S->procs_len) {
    procedure * temp = realloc(S->procs, new_size);
    if(!temp) { abort(); }
    S->procs = temp;
  }
}

static void consume_instruction(day_4_state * S) {
  resize_instructions(S);
  procedure * p = &(S->procs[S->procs_len]);
  p->count = consume_move(S); consume_space(S);
  p->src = consume_from(S); consume_space(S);
  p->dest = consume_to(S);
  S->procs_len++;
}

char pop_crate(size_t column) {
  for(size_t i = 0; i < MAX_ROWS; i++) {
    char * C = &(crates[i][column]);
    if(*C != EMPTY && *C != '\0') {
      char T = *C;
      *C = EMPTY;
      return T;
    }
  }

  fprintf(stderr, "welp, i dun messed up.\n");
  exit(EXIT_FAILURE);
}

static void push_crate(size_t column, char crate) {
  char * P = NULL, * C = NULL;
  for(size_t i = 0; i < MAX_ROWS; i++) {
    if(i > 0) { P = &(crates[i - 1][column]); }
    C = &(crates[i][column]);
    if(*C != EMPTY && *C != '\0') {
      *P = crate;
      return;
    }
  }

  *C = crate;
}

static void initialize_crate(size_t column, char crate) {
  char * C = &(crates[row_len][column]);
  *C = crate;
}

static void consume_empty_crate(day_4_state * S) {
  size_t count = 0;
  while(isspace(**S->str)) {
    (*S->str)++;
    count++;
    if(count % 4 == 0 || count % 5 == 0) {
      initialize_crate(current, EMPTY);
      current++;
      count = 0;
    }
  }
}

static void consume_crate(day_4_state * S) {
  if(**(S->str) == '[') {
    (*(S->str))++;
    initialize_crate(current, **(S->str));
    current++;
  }
}

static void consume_crates(day_4_state * S) {
  do {
    consume_empty_crate(S);
    consume_crate(S);
    (*(S->str))++;
  } while(*(S->str) && **(S->str) != '\0');
}

static void aoc_day4_p0_worker(const char * line, ssize_t read, void * state) {
  day_4_state * S = (day_4_state *)state;
  S->str = &line;
  if(read > 1) {
    if(*line == 'm') {
      consume_instruction(S);
    } else if(strncmp(line, " 1 ", 3) != 0) {
      /* parse crates */
      consume_crates(S);
      columns_len = current > columns_len ? current : columns_len;
      current = 0;
      row_len++;
    }
  }
}

int aoc_day4_p0(int argc, char **argv) {
  day_4_state state = {
    .procs_sz = 1
  };

  int result = read_lines(argc, argv, &state, &aoc_day4_p0_worker);
  if(result == EXIT_SUCCESS) {
    if(state.procs_len > 0) {
      for(size_t i = 0; i < state.procs_len; i++) {
        const procedure p = state.procs[i];
        for(uint32_t c = 0; c < p.count; c++) {
          push_crate(p.dest - 1, pop_crate(p.src - 1));
        }
      }
    }
    char buf[32] = { 0 }, * s = buf;
    for(size_t i = 0; i < columns_len; i++) {
      *(s++) = pop_crate(i);
    }
    fprintf(stdout, "Stack Tops: %s\n", buf);
  }
  return result;
}

/*
--- Part Two ---

As you watch the crane operator expertly rearrange the crates, you notice the
process isn't following your prediction.

Some mud was covering the writing on the side of the crane, and you quickly
wipe it away. The crane isn't a CrateMover 9000 - it's a CrateMover 9001.

The CrateMover 9001 is notable for many new and exciting features: air
conditioning, leather seats, an extra cup holder, and the ability to pick up
and move multiple crates at once.

Again considering the example above, the crates begin in the same
configuration:

    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

Moving a single crate from stack 2 to stack 1 behaves the same as before:

[D]
[N] [C]
[Z] [M] [P]
 1   2   3

However, the action of moving three crates from stack 1 to stack 3 means that
those three moved crates stay in the same order, resulting in this new
configuration:

        [D]
        [N]
    [C] [Z]
    [M] [P]
 1   2   3

Next, as both crates are moved from stack 2 to stack 1, they retain their order
as well:

        [D]
        [N]
[C]     [Z]
[M]     [P]
 1   2   3

Finally, a single crate is still moved from stack 1 to stack 2, but now it's
crate C that gets moved:

        [D]
        [N]
        [Z]
[M] [C] [P]
 1   2   3

In this example, the CrateMover 9001 has put the crates in a totally different
order: MCD.

Before the rearrangement process finishes, update your simulation so that the
Elves know where they should stand to be ready to unload the final supplies.
After the rearrangement procedure completes, what crate ends up on top of each
stack?
*/

int aoc_day4_p1(int argc, char **argv) {
  day_4_state state = {
    .procs_sz = 1
  };

  int result = read_lines(argc, argv, &state, &aoc_day4_p0_worker);
  if(result == EXIT_SUCCESS) {
    if(state.procs_len > 0) {
      for(size_t i = 0; i < state.procs_len; i++) {
        const procedure p = state.procs[i];
        char temp_stack[64] = { 0 }, * T = temp_stack;
        for(uint32_t c = 0; c < p.count; c++) {
          *(T++) = pop_crate(p.src - 1);
        }
        while(T-- >= temp_stack) {
          push_crate(p.dest - 1, *T);
        }
      }
    }

    char buf[32] = { 0 }, * s = buf;
    for(size_t i = 0; i < columns_len; i++) {
      *(s++) = pop_crate(i);
    }
    fprintf(stdout, "Ordered Stack Tops: %s\n", buf);
  }
  return result;
}

