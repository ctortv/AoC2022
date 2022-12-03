#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "../include/day2.h"

/*
One Elf has the important job of loading all of the rucksacks with supplies
for the jungle journey. Unfortunately, that Elf didn't quite follow the
packing instructions, and so a few items now need to be rearranged.

Each rucksack has two large compartments. All items of a given type are meant
to go into exactly one of the two compartments. The Elf that did the packing
failed to follow this rule for exactly one item type per rucksack.

The Elves have made a list of all of the items currently in each rucksack (your
puzzle input), but they need your help finding the errors. Every item type is
identified by a single lowercase or uppercase letter (that is, a and A refer to
different types of items).

The list of items for each rucksack is given as characters all on a single
line. A given rucksack always has the same number of items in each of its two
compartments, so the first half of the characters represent items in the first
compartment, while the second half of the characters represent items in the
second compartment.

For example, suppose you have the following list of contents from six
rucksacks:

vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw

    The first rucksack contains the items vJrwpWtwJgWrhcsFMMfFFhFp, which means
    its first compartment contains the items vJrwpWtwJgWr, while the second
    compartment contains the items hcsFMMfFFhFp. The only item type that
    appears in both compartments is lowercase p.

    The second rucksack's compartments contain jqHRNqRjqzjGDLGL and
    rsFMfFZSrLrFZsSL. The only item type that appears in both compartments is
    uppercase L.

    The third rucksack's compartments contain PmmdzqPrV and vPwwTWBwg; the only
    common item type is uppercase P.

    The fourth rucksack's compartments only share item type v.

    The fifth rucksack's compartments only share item type t.

    The sixth rucksack's compartments only share item type s.

To help prioritize item rearrangement, every item type can be converted to a
priority:

    Lowercase item types a through z have priorities 1 through 26.
    Uppercase item types A through Z have priorities 27 through 52.

In the above example, the priority of the item type that appears in both
compartments of each rucksack is 16 (p), 38 (L), 42 (P), 22 (v), 20 (t), and 19
(s); the sum of these is 157.

Find the item type that appears in both compartments of each rucksack. What is
the sum of the priorities of those item types?

*/

int aoc_day2_p0(int argc, char **argv) {
  if(argc < 4) { goto err0; }

  const char * input = argv[3];

  FILE * fp = fopen(input, "r");
  if(!fp || errno) { goto err1; }

  char * line = NULL;
  size_t len = 0;
  ssize_t read = 0;

  int total_score = 0;
  while((read = getline(&line, &len, fp)) != -1) {
    static const size_t max_line_len = /* arbitrary: */ 1024;
    if(errno) { goto err2; }
    if(read <= 1) { continue; }

    const size_t line_len = strnlen(line, max_line_len);
    if(line_len < 1) { continue; }

    const size_t mid_point = line_len / 2;
    const char * const end = line + read;

    const char * lhs = line;

    const char * match = NULL;
    const char * mid_start = line + mid_point;
    do {
      const char * rhs = mid_start;
      do {
        if(*lhs == *rhs) {
          match = lhs;
          goto done;
        }
      } while(rhs++ < end);
    } while(lhs++ < mid_start);

done: ;
    const char item = *match;
    total_score += item <= 'Z'
      /* Score uppercase */
      ? (item - 'A') + 27
      /* Score lowercase */
      : (item - 'a') + 1;
  }

  fprintf(stdout, "Rucksack Priorities: %i\n", total_score);

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

