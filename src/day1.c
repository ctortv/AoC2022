#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "../include/utils.h"
#include "../include/day1.h"

typedef struct {
  int total_score;
} day_1_state;

static int adjust_score(const int key, const int opponent_score);

/*
--- Day 2: Rock Paper Scissors - Part 1 ---

Appreciative of your help yesterday, one Elf gives you an encrypted strategy
guide (your puzzle input) that they say will be sure to help you win. "The
first column is what your opponent is going to play: A for Rock, B for
Paper, and C for Scissors. The second column--" Suddenly, the Elf is called
away to help with someone's tent.

The second column, you reason, must be what you should play in response: X for
Rock, Y for Paper, and Z for Scissors. Winning every time would be suspicious,
so the responses must have been carefully chosen.

The winner of the whole tournament is the player with the highest score. Your
total score is the sum of your scores for each round. The score for a single
round is the score for the shape you selected (1 for Rock, 2 for Paper, and 3
for Scissors) plus the score for the outcome of the round (0 if you lost, 3 if
the round was a draw, and 6 if you won).

Since you can't be sure if the Elf is trying to help you or trick you, you
should calculate the score you would get if you were to follow the strategy
guide.

For example, suppose you were given the following strategy guide:

A Y
B X
C Z

This strategy guide predicts and recommends the following:

    In the first round, your opponent will choose Rock (A), and you should
    choose Paper (Y). This ends in a win for you with a score of 8 (2 because
    you chose Paper + 6 because you won).

    In the second round, your opponent will choose Paper (B), and you should
    choose Rock (X). This ends in a loss for you with a score of 1 (1 + 0).

    The third round is a draw with both players choosing Scissors, giving you a
    score of 3 + 3 = 6.

In this example, if you were to follow the strategy guide, you would get a
total score of 15 (8 + 1 + 6).

What would your total score be if everything goes exactly according to your
strategy guide?
*/

static void aoc_day1_p0_worker(const char * line, ssize_t read, void * state) {
  day_1_state * S = (day_1_state *)state;
  const char * const end = line + read;
  const char my_shape = *(end - 2);
  const char their_shape = *line;
  /* my shapes are X..Z, worth 1..3 points, respectively */
  const int shape_score = (my_shape - 'Z') + 3;
  /* their shapes are A..C, worth 1..3 points, respectively */
  const int opponent_score = (their_shape - 'C') + 3;

  S->total_score += shape_score;

  const int diff = shape_score - opponent_score;
  /* 3 points for a tie... */
  if(diff == 0) { S->total_score += 3; }
  /* 6 points for a win */
  else if(diff == 1 || diff == -2) { S->total_score += 6; }
}

int aoc_day1_p0(int argc, char **argv) {
  day_1_state state = { 0 };
  int result = read_lines(argc, argv, &state, &aoc_day1_p0_worker);
  fprintf(stdout, "RPS Total: %i\n", state.total_score);
  return result;
}

/*
--- Part Two ---

The Elf finishes helping with the tent and sneaks back over to you. "Anyway,
the second column says how the round needs to end: X means you need to lose, Y
means you need to end the round in a draw, and Z means you need to win. Good
luck!"

The total score is still calculated in the same way, but now you need to figure
out what shape to choose so the round ends as indicated. The example above now
goes like this:

    In the first round, your opponent will choose Rock (A), and you need the
    round to end in a draw (Y), so you also choose Rock. This gives you a score
    of 1 + 3 = 4.

    In the second round, your opponent will choose Paper (B), and you choose
    Rock so you lose (X) with a score of 1 + 0 = 1.

    In the third round, you will defeat your opponent's Scissors with Rock for
    a score of 1 + 6 = 7.

Now that you're correctly decrypting the ultra top secret strategy guide, you
would get a total score of 12.

Following the Elf's instructions for the second column, what would your total
score be if everything goes exactly according to your strategy guide?
*/

static void aoc_day1_p1_worker(const char * line, ssize_t read, void * state) {
  day_1_state * S = (day_1_state  *)state;
  const char * const end = line + read;
  const char my_shape = *(end - 2);
  const char their_shape = *line;

  /* their shapes are A..C, worth 1..3 points, respectively */
  const int opponent_score = (their_shape - 'C') + 3;
  /* adjust per the strategy... */
  int shape_score = adjust_score((my_shape - 'Z') + 3, opponent_score);

  S->total_score += shape_score;

  const int diff = shape_score - opponent_score;
  /* 3 points for a tie... */
  if(diff == 0) { S->total_score += 3; }
  /* 6 points for a win */
  else if(diff == 1 || diff == -2) { S->total_score += 6; }
}

int aoc_day1_p1(int argc, char **argv) {
  day_1_state state = { 0 };
  int result = read_lines(argc, argv, &state, &aoc_day1_p1_worker);
  fprintf(stdout, "Strategy Total: %i\n", state.total_score);
  return result;
}

static int adjust_score(const int key, const int opponent_score) {
  enum { lose = 1, draw, win };
  enum { rock = 1, paper, scissors };
  switch(key) {
    case win:
      if(opponent_score == rock) { return paper; }
      else if(opponent_score == paper) {return scissors; }
      return rock;
    case lose:
      if(opponent_score == rock) { return scissors; }
      else if(opponent_score == paper) { return rock; }
      return paper;
    case draw:
    default:
      return opponent_score;
  }
}
