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
