#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../include/utils.h"

/*
 * welp. I couldn't solve this on my own, so I ended up stealing someone else's
 * solution like a filthy little cheater and adapting it to C. See:
 * https://github.com/betaveros/advent-of-code-2022/blob/main/p9.noul.
 *
 * I kept _some_ of my original garbage code at the bottom.
*/

typedef struct {
  int x, y;
} vertex;

typedef struct {
  char dir;
  char padding[3];
  uint32_t moves;
} rule;

/* from a previous effort
 *
  typedef struct {
    int x, y;
    bool visited;
    char padding[7];
  } visitor;
*/

typedef struct {
  /* from a previous effort */
  //* visitor ** trail;
  //* visitor * trail_data;
  //* visitor ** vtrail;
  //* visitor * vtrail_data;
  //* visitor * H;
  //* visitor * T;

  size_t input_len;
  char * input;

  size_t rules_len;
  rule * rules;
} day8_state;

static size_t visits_len = 0;
static vertex visits[65536] = { 0 };

static void read_instructions(const char * line, ssize_t read, void * state) {
  if(read <= 0 || !line) { return; }
  day8_state * S = (day8_state *)state;
  size_t len = 0;
  line_cp(line, &len, &S->input, &S->input_len);
  S->rules_len++;
}

static void alloc_rules(day8_state * S) {
  S->rules = calloc(S->rules_len, sizeof * S->rules);
  if(!S->rules) { abort(); }
  const char * str = S->input;
  rule * R = S->rules;
  const rule * end = S->rules + S->rules_len;
  while(*str != '\0' && R < end) {
    R->dir = *str++;
    ws(&str);
    R->moves = read_uint(&str);
    ws(&str);
    R++;
  }
}

static void move(const rule * R, vertex * v) {
  switch(R->dir) {
    case 'R': v->y =  1; v->x = 0; return;
    case 'L': v->y = -1; v->x = 0; return;
    case 'U': v->x = -1; v->y = 0; return;
    case 'D': v->x =  1; v->y = 0; return;
    default: abort();
  }
}


static void walk_trail(day8_state * S) {
  vertex H = { 0 },
         T = { 0 };
  visits[visits_len++] = T;

  const rule * R = S->rules;
  const rule * end = S->rules + S->rules_len;
  do {
    for(uint32_t m = 0; m < R->moves; m++) {
      vertex h = H;
      move(R, &h);
      H.x += h.x;
      H.y += h.y;

      vertex temp = { 0 },
        diff = {
          .x = T.x - H.x,
          .y = T.y - H.y
        };

      if(T.x - H.x == 2)       { temp.x =  1; }
      else if(T.x - H.x == -2) { temp.x = -1; }
      else if(T.y - H.y ==  2) { temp.y =  1; }
      else if(T.y - H.y == -2) { temp.y = -1; }
      else { temp = diff; }

      T.x = H.x + temp.x;
      T.y = H.y + temp.y;

      bool found = false;
      for(size_t i = 0; i < visits_len; i++) {
        if(visits[i].x == T.x && visits[i].y == T.y) {
          found = true;
          break;
        }
      }
      if(!found) {
        visits[visits_len++] = T;
      }
    }
  } while(++R < end);
}

/*
--- Day 9: Rope Bridge ---

This rope bridge creaks as you walk along it. You aren't sure how old it is, or
whether it can even support your weight.

It seems to support the Elves just fine, though. The bridge spans a gorge which
was carved out by the massive river far below you.

You step carefully; as you do, the ropes stretch and twist. You decide to
distract yourself by modeling rope physics; maybe you can even figure out where
not to step.

Consider a rope with a knot at each end; these knots mark the head and the tail
of the rope. If the head moves far enough away from the tail, the tail is
pulled toward the head.

Due to nebulous reasoning involving Planck lengths, you should be able to model
the positions of the knots on a two-dimensional grid. Then, by following a
hypothetical series of motions (your puzzle input) for the head, you can
determine how the tail will move.

Due to the aforementioned Planck lengths, the rope must be quite short; in
fact, the head (H) and tail (T) must always be touching (diagonally adjacent
and even overlapping both count as touching):

....
.TH.
....

....
.H..
..T.
....

...
.H. (H covers T)
...

If the head is ever two steps directly up, down, left, or right from the tail,
the tail must also move one step in that direction so it remains close enough:

.....    .....    .....
.TH.. -> .T.H. -> ..TH.
.....    .....    .....

...    ...    ...
.T.    .T.    ...
.H. -> ... -> .T.
...    .H.    .H.
...    ...    ...

Otherwise, if the head and tail aren't touching and aren't in the same row or
column, the tail always moves one step diagonally to keep up:

.....    .....    .....
.....    ..H..    ..H..
..H.. -> ..... -> ..T..
.T...    .T...    .....
.....    .....    .....

.....    .....    .....
.....    .....    .....
..H.. -> ...H. -> ..TH.
.T...    .T...    .....
.....    .....    .....

You just need to work out where the tail goes as the head follows a series of
motions. Assume the head and the tail both start at the same position,
overlapping.

For example:

R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2

This series of motions moves the head right four steps, then up four steps,
then left three steps, then down one step, and so on. After each step, you'll
need to update the position of the tail if the step means the head is no longer
adjacent to the tail. Visually, these motions occur as follows (s marks the
starting position as a reference point):

== Initial State ==

......
......
......
......
H.....  (H covers T, s)

== R 4 ==

......
......
......
......
TH....  (T covers s)

......
......
......
......
sTH...

......
......
......
......
s.TH..

......
......
......
......
s..TH.

== U 4 ==

......
......
......
....H.
s..T..

......
......
....H.
....T.
s.....

......
....H.
....T.
......
s.....

....H.
....T.
......
......
s.....

== L 3 ==

...H..
....T.
......
......
s.....

..HT..
......
......
......
s.....

.HT...
......
......
......
s.....

== D 1 ==

..T...
.H....
......
......
s.....

== R 4 ==

..T...
..H...
......
......
s.....

..T...
...H..
......
......
s.....

......
...TH.
......
......
s.....

......
....TH
......
......
s.....

== D 1 ==

......
....T.
.....H
......
s.....

== L 5 ==

......
....T.
....H.
......
s.....

......
....T.
...H..
......
s.....

......
......
..HT..
......
s.....

......
......
.HT...
......
s.....

......
......
HT....
......
s.....

== R 2 ==

......
......
.H....  (H covers T)
......
s.....

......
......
.TH...
......
s.....

After simulating the rope, you can count up all of the positions the tail
visited at least once. In this diagram, s again marks the starting position
(which the tail also visited) and # marks other positions the tail visited:

..##..
...##.
.####.
....#.
s###..

So, there are 13 positions the tail visited at least once.

Simulate your complete hypothetical series of motions. How many positions does
the tail of the rope visit at least once?

*/

int aoc_day8_p0(int argc, char **argv) {
  day8_state S = { 0 };
  int result = read_lines(argc, argv, &S, &read_instructions);
  S.rules_len--;

  alloc_rules(&S);
  walk_trail(&S);

  fprintf(stdout, "Total: %zu\n", visits_len);

  return result;
}

/*
This is from a previous attempt before I gave up :( Keeping for LOLs.
Won't compile; won't work.
*  //alloc_trail(&(S.trail), &(S.trail_data));
*  //alloc_trail(&(S.vtrail), &(S.vtrail_data));
*
* S.H = &(S.trail)[max_grid_x / 2][max_grid_y / 2];
* S.T = &(S.vtrail)[max_grid_x / 2][max_grid_y / 2];;
* S.T->visited = true;
* S.H->visited = true;
*
* static void alloc_trail(visitor *** trail, visitor ** data) {
*   *trail = calloc(max_grid_x, sizeof *** trail);
*   if(!*trail) { abort(); }
*   *data = calloc(max_grid_x * max_grid_y, sizeof ** data);
*   if(!*data) { abort(); }
*   for(int i = 0; i < max_grid_x; i++) {
*     visitor * p = (*data) + i * max_grid_x;
*     (*trail)[i] = p;
*   }

*   for(int x = 0; x < max_grid_x; x++) {
*     for(int y = 0; y < max_grid_y; y++) {
*       visitor * v = &(*trail)[x][y];
*       v->x = x;
*       v->y = y;
*     }
*   }
* }

* static void print_grid(visitor ** trail, char m) {
*   for(int x = 0; x < max_grid_x; x++) {
*     for(int y = 0; y < max_grid_y; y++) {
*       const visitor * v = &trail[x][y];
*       if(v->visited) {
*         fprintf(stdout, "%c", m);
*       }
*       else fprintf(stdout, ".");
*     }
*     fprintf(stdout, "\n");
*   }
* }
*/


