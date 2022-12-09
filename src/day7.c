#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../include/utils.h"
#include "../include/day7.h"

typedef struct node node;
typedef struct node {
  int height;
  char padding[4];
  node * N, * S, * E, * W;
} node;

typedef enum dir {
  NORTH, SOUTH, EAST, WEST
} dir;

typedef struct day7_state {
  node ** nodes;
  node * node_data;
  size_t rows, cols;
  size_t map_len;
  char * map;
} day7_state;

static void free_state(day7_state * S);

static void build_map(const char * line, ssize_t read, void * state) {
  day7_state * S = (day7_state *)state;
  (void)state;
  if(read <= 0 || !line) { return; }
  size_t len = 0;
  line_cp(line, &len, &S->map, &S->map_len);
  if(len > 0) {
    if(len > 1 && S->cols <= 0) { S->cols = len - 1; }
    S->rows++;
  }
}

static void alloc_nodes(day7_state * S) {
  S->nodes = calloc(S->rows, sizeof ** S->nodes);
  S->node_data = calloc(S->rows * S->cols, sizeof * S->node_data);
  for(size_t i = 0; i < S->rows; i++) {
     S->nodes[i] = S->node_data + i * S->rows;
  }
}

static inline const node * switch_node(const node * p, dir d) {
  switch(d) {
    case NORTH: return p->N;
    case SOUTH: return p->S;
    case EAST:  return p->E;
    case WEST:  return p->W;
    default: abort();
  }
}

static size_t score_nodes(const node * n, const node * p, dir d) {
  if(!p) { return 0; }
  size_t score = 0;
  while(p) {
    score++;
    if(p->height >= n->height) {
      return score;
    }
    p = switch_node(p, d);
  }
  return score;
}

static size_t walk_scored_nodes(day7_state * S) {
  size_t score = 0;
  node * n = S->node_data,
       * last = &S->node_data[S->rows * S->cols];
  do {
    size_t tree_score =
        score_nodes(n, n->N, NORTH)
      * score_nodes(n, n->S, SOUTH)
      * score_nodes(n, n->E, EAST)
      * score_nodes(n, n->W, WEST);
    score = tree_score > score
      ? tree_score
      : score;
  } while(++n < last);
  return score;
}

static bool find_count(const node * n, const node * p, dir d, size_t * count) {
  bool smol = p == NULL;
  while(p && (smol = p->height < n->height)) {
    p = switch_node(p, d);
  }
  if(smol) { (*count)++; }
  return smol;
}

static size_t walk_counted_nodes(day7_state * S) {
  node * n = S->node_data,
       * last = &S->node_data[S->rows * S->cols];
  size_t count = 0;
  do {
    if(find_count(n, n->N, NORTH, &count)) continue;
    if(find_count(n, n->S, SOUTH, &count)) continue;
    if(find_count(n, n->E, EAST, &count)) continue;
    if(find_count(n, n->W, WEST, &count)) continue;
  } while(++n < last);
  return count;
}

static void build_nodes(day7_state * S) {
  const char * s = S->map;
  for(size_t x = 0; x < S->rows; x++) {
    node * N = S->nodes[x],
         * P = x > 0 ? S->nodes[x - 1] : NULL;
    for(size_t y = 0; y < S->cols; y++) {
      if(*s == '\n') { break; }
      node * n = &N[y];
      /* bind N & S */
      if(P) {
        n->N = &P[y];
        P[y].S = n;
      }
      /* ... and E & W: */
      if(y > 0) {
        n->W = &N[y - 1];
        N[y - 1].E = n;
      }
      if(y < S->cols - 1) {
        n->E = &N[y + 1];
        N[y + 1].W = n;
      }

      /* height from 0 - 9 */
      n->height = (*s) - '0';
      s++;
    }
    s++;
  }
}

/*
--- Day 8: Treetop Tree House ---

The expedition comes across a peculiar patch of tall trees all planted
carefully in a grid. The Elves explain that a previous expedition planted these
trees as a reforestation effort. Now, they're curious if this would be a good
location for a tree house.

First, determine whether there is enough tree cover here to keep a tree house
hidden. To do this, you need to count the number of trees that are visible from
outside the grid when looking directly along a row or column.

The Elves have already launched a quadcopter to generate a map with the height
of each tree (your puzzle input). For example:

30373
25512
65332
33549
35390

Each tree is represented as a single digit whose value is its height, where 0
is the shortest and 9 is the tallest.

A tree is visible if all of the other trees between it and an edge of the grid
are shorter than it. Only consider trees in the same row or column; that is,
only look up, down, left, or right from any given tree.

All of the trees around the edge of the grid are visible - since they are
already on the edge, there are no trees to block the view. In this example,
that only leaves the interior nine trees to consider:

    The top-left 5 is visible from the left and top. (It isn't visible from the
    right or bottom since other trees of height 5 are in the way.)

    The top-middle 5 is visible from the top and right.

    The top-right 1 is not visible from any direction; for it to be visible,
    there would need to only be trees of height 0 between it and an edge.

    The left-middle 5 is visible, but only from the right.

    The center 3 is not visible from any direction; for it to be visible, there
    would need to be only trees of at most height 2 between it and an edge.

    The right-middle 3 is visible from the right.

    In the bottom row, the middle 5 is visible, but the 3 and 4 are not.

With 16 trees visible on the edge and another 5 visible in the interior, a
total of 21 trees are visible in this arrangement.

Consider your map; how many trees are visible from outside the grid?
*/
int aoc_day7_p0(int argc, char **argv) {
  day7_state S = { 0 };
  int result = read_lines(argc, argv, &S, &build_map);
  S.rows--; /* ugh */
  alloc_nodes(&S);
  build_nodes(&S);

  size_t count = walk_counted_nodes(&S);
  fprintf(stdout, "Visible Trees: %zu\n", count);
  free_state(&S);

  return result;
}

/*
--- Part Two ---

Content with the amount of tree cover available, the Elves just need to know
the best spot to build their tree house: they would like to be able to see a
lot of trees.

To measure the viewing distance from a given tree, look up, down, left, and
right from that tree; stop if you reach an edge or at the first tree that is
the same height or taller than the tree under consideration. (If a tree is
right on the edge, at least one of its viewing distances will be zero.)

The Elves don't care about distant trees taller than those found by the rules
above; the proposed tree house has large eaves to keep it dry, so they wouldn't
be able to see higher than the tree house anyway.

In the example above, consider the middle 5 in the second row:

30373
25512
65332
33549
35390

    Looking up, its view is not blocked; it can see 1 tree (of height 3).

    Looking left, its view is blocked immediately; it can see only 1 tree (of
    height 5, right next to it).

    Looking right, its view is not blocked; it can see 2 trees.

    Looking down, its view is blocked eventually; it can see 2 trees (one of
    height 3, then the tree of height 5 that blocks its view).

A tree's scenic score is found by multiplying together its viewing distance in
each of the four directions. For this tree, this is 4 (found by multiplying 1 *
1 * 2 * 2).

However, you can do even better: consider the tree of height 5 in the middle of
the fourth row:

30373
25512
65332
33549
35390

    Looking up, its view is blocked at 2 trees (by another tree with a height
    of 5).

    Looking left, its view is not blocked; it can see 2 trees.

    Looking down, its view is also not blocked; it can see 1 tree.

    Looking right, its view is blocked at 2 trees (by a massive tree of height
    9).

This tree's scenic score is 8 (2 * 2 * 1 * 2); this is the ideal spot for the
tree house.

Consider each tree on your map. What is the highest scenic score possible for
any tree?
*/
int aoc_day7_p1(int argc, char **argv) {
  day7_state S = { 0 };
  int result = read_lines(argc, argv, &S, &build_map);
  S.rows--; /* ugh */
  alloc_nodes(&S);
  build_nodes(&S);

  size_t score = walk_scored_nodes(&S);
  fprintf(stdout, "Highest Scenic Score: %zu\n", score);
  free_state(&S);

  return result;
}

static void free_state(day7_state * S) {
  free(S->nodes), S->nodes = NULL;
  free(S->node_data), S->node_data = NULL;
  free(S->map), S->map = NULL;
}
