#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

#include "../include/day2.h"

static const size_t max_line_len = /* arbitrary: */ 1024;

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

/*
--- Part Two ---

As you finish identifying the misplaced items, the Elves come to you with
another issue.

For safety, the Elves are divided into groups of three. Every Elf carries a
badge that identifies their group. For efficiency, within each group of three
Elves, the badge is the only item type carried by all three Elves. That is, if
a group's badge is item type B, then all three Elves will have item type B
somewhere in their rucksack, and at most two of the Elves will be carrying any
other item type.

The problem is that someone forgot to put this year's updated authenticity
sticker on the badges. All of the badges need to be pulled out of the rucksacks
so the new authenticity stickers can be attached.

Additionally, nobody wrote down which item type corresponds to each group's
badges. The only way to tell which item type is the right one is by finding the
one item type that is common between all three Elves in each group.

Every set of three lines in your list corresponds to a single group, but each
group can have a different badge item type. So, in the above example, the first
group's rucksacks are the first three lines:

vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg

And the second group's rucksacks are the next three lines:

wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw

In the first group, the only item type that appears in all three rucksacks is
lowercase r; this must be their badges. In the second group, their badge item
type must be Z.

Priorities for these items must still be found to organize the sticker
attachment efforts: here, they are 18 (r) for the first group and 52 (Z) for
the second group. The sum of these is 70.

Find the item type that corresponds to the badges of each three-Elf group. What
is the sum of the priorities of those item types?
*/

static int group_comparer(const void * l, const void * r)
{
  const char * const * lhs = l, * const * rhs = r;
  size_t l_len = strnlen(*lhs, max_line_len);
  size_t r_len = strnlen(*rhs, max_line_len);
  size_t diff = l_len > r_len ? l_len - r_len : l_len - r_len;
  return diff == 0 ? 0 : diff > INT_MAX ? -1 : 1;
}

int aoc_day2_p1(int argc, char **argv) {
  if(argc < 4) { goto err0; }

  const char * input = argv[3];

  FILE * fp = fopen(input, "r");
  if(!fp || errno) { goto err1; }

  char * line = NULL;
  size_t len = 0;
  ssize_t read = 0;

  /* horrible, but whatev */
  char * groups[3] = { 0 };

  int total_score = 0;
  size_t index = 0;
  while((read = getline(&line, &len, fp)) != -1) {
    if(errno) { goto err2; }
    if(read <= 1) { continue; }

    const size_t line_len = strnlen(line, max_line_len);

    const char * match = NULL;
    groups[index] = calloc(line_len, sizeof groups[0]);
    if(!groups[index]) { abort(); }

    memmove(groups[index], line, line_len);

    index++;
    if(index % 3 == 0) {
      const char * sorted[3] = { groups[0], groups[1], groups[2] };

      qsort(sorted, 3, sizeof * sorted, group_comparer);
      const char * first = sorted[2];
      const char * fend = first + strnlen(first, max_line_len);

      do {
        const char * second = sorted[1];
        const char * send = second + strnlen(second, max_line_len);
        do {
          const char * third = sorted[0];
          const char * tend = third + strnlen(third, max_line_len);
          do {
            if(*first == *second && *second == *third) {
              match = first;
              goto done;
            }
          } while(third++ < tend);
        } while(second++ < send);
      } while(first++ < fend);

done:
      index = 0;
      const char item = *match;

      int badge = item <= 'Z'
        // Score uppercase
        ? (item - 'A') + 27
        // Score lowercase
        : (item - 'a') + 1;

      total_score += badge;

      free(groups[0]), free(groups[1]), free(groups[2]);
    }
  }

  fprintf(stdout, "Common Rucksack Priorities: %i\n", total_score);

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

