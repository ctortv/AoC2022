#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>

#include "../include/utils.h"
#include "../include/day0.h"

typedef int (*aoc_day_fn)(int, char **);


static const aoc_day_fn aoc_days[][2] = {
  { aoc_day0_p0, NULL },
};

int main(int argc, char **argv) {
  static const size_t available_days = sizeof aoc_days / sizeof aoc_days[0];
  static const bool exit_on_fail = true;

  if(argc < 3) { goto err0; }

  const unsigned long day = parse_arg(argv[1], exit_on_fail);
  if(day < 0 || day > 30) { goto err1; }
  if(day > available_days - 1) { goto err2; }

  const unsigned long puzzle = parse_arg(argv[2], exit_on_fail);
  if(puzzle < 0 || puzzle > 1) { goto err3; }

  if(!aoc_days[day][puzzle]) { goto err4; }
  return aoc_days[day][puzzle](argc, argv);

err4:
  fprintf(stderr, "i haven't completed the %s puzzle for day %zu. sorry.\n", puzzle == 0 ? "first" : "second", day);
  goto err;

err3:
  fprintf(stderr, "i support up to two puzzles a day, friend. if i even get that far. try 0 or 1.\n");
  goto err;

err2:
  fprintf(stderr, "i haven't gotten to that day, yet. i only have %zu day(s).\n", available_days);
  fprintf(stderr, "try 'aoc %zu 0' for the most recent day/puzzle.\n", available_days - 1);
  goto err;

err1:
  fprintf(stderr, "last i checked, there were only 31 days in December but idk.\n%s is invalid; we're zero-based around here.\n", argv[1]);
  goto err;

err0:
  fprintf(stderr, "hey, tell me your day and puzzle!\nzero-based cuz we're all friends here.\n");
  fprintf(stderr, "usage: aoc <day> <puzzle> <puzzle_args>\nexample: aoc %zu %i\n", available_days - 1, 0);
  goto err; /* no fall-through */

err:
  return -1;
}

