/*
 * evenodd - Prove that you're able to count up to 3, determine
 * parity, associate it with a key, and act accordingly within a time
 * limit.  Repeatedly.
 *
 * A number of leds will turn on.  If this number is even, or zero,
 * press the 'e' key.  If this number is odd, press the 'o' key.
 *
 * Every time you get this right, your score increases.  Every time
 * you get this wrong, your score decreases.
 *
 * There is also a time limit that depends on the current level.  When
 * this limit is reached, your score decreases and a new combination
 * of leds, possibly the same as the last combination, will follow.
 *
 * You start at level 1.  When your score is high enough, you move
 * automatically to the next level.  You never return to previous
 * levels.
 *
 * The aim of the game is to attain the highest score possible.
 *
 * Good luck.  Oh, and to quit the game, press the 'q' key.
 */
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <time.h>

#include "common.h"
#include "led.h"

enum {
      INITIALDELAY = 50,
      LEVELTIMEFACTOR = 5,
      LEVELUPFACTOR = 10
};

enum {
      HIT,
      MISS,
      TIMELIMIT,
      QUIT
};

struct game
{
  int problem;
  int hits;
  int misses;
  int score;
  int level;
  int quit;
};

void init(struct game *g);
int ongoing(struct game *g);
void newproblem(struct game *g);
int inputaction(struct game *g);
void gameover(struct game *g);
void hit(struct game *g);
void miss(struct game *g);
void decay(struct game *g);
void incscore(struct game *g);
void decscore(struct game *g);
int levelthreshold(struct game *g);
void inclevel(struct game *g);
void display(struct game *g);
void stats(struct game *g);

int main(void)
{
  struct game g;

  ledapp_setup();

  setlocale(LC_ALL, "");
  initscr();
  noecho();

  srand((unsigned int)time(NULL));

  init(&g);

  while (ongoing(&g)) {
    newproblem(&g);
    display(&g);
    switch (inputaction(&g)) {
    case HIT:
      hit(&g);
      break;
    case MISS:
      miss(&g);
      break;
    case TIMELIMIT:
      decay(&g);
      break;
    case QUIT:
    default:
      gameover(&g);
      break;
    }
  }

  endwin();

  stats(&g);

  ledapp_teardown();
}

void init(struct game *g)
{
  g->problem = 0;
  g->hits = 0;
  g->misses = 0;
  g->score = 0;
  g->level = 0;
  g->quit = 0;
  inclevel(g);
}

void newproblem(struct game *g)
{
  g->problem = rand() & 7;
  led_reset(g->problem);
}

int inputaction(struct game *g)
{
  /* 0x96 is 0b10010110, the parity for each case. */
  int odd = 0x96 & (1 << g->problem);
  while (1) {
    switch (getch()) {
    case 'e':
      return odd ? MISS : HIT;
    case 'o':
      return odd ? HIT : MISS;
    case 'q':
      return QUIT;
    case ERR:
      return TIMELIMIT;
    default:
      /* Must be done to prevent cheating. */
      return MISS;
    }
  }
}

int ongoing(struct game *g)
{
  return !g->quit;
}

void gameover(struct game *g)
{
  g->quit = 1;
}

void hit(struct game *g)
{
  g->hits++;
  incscore(g);
}

void miss(struct game *g)
{
  g->misses++;
  decscore(g);
}

void decay(struct game *g)
{
  decscore(g);
}

void incscore(struct game *g)
{
  g->score++;
  if (g->score >= levelthreshold(g)) {
    inclevel(g);
  }
}

void decscore(struct game *g)
{
  if (g->score > 0) {
    g->score--;
  }
}

int levelthreshold(struct game *g)
{
  return g->level * LEVELUPFACTOR;
}

void inclevel(struct game *g)
{
  int nextlevel = g->level + 1;
  int tenths = INITIALDELAY - nextlevel * LEVELTIMEFACTOR;
  if (tenths > 0) {
    g->level = nextlevel;
    halfdelay(tenths);
  } else {
    gameover(g);
  }
}

void display(struct game *g)
{
  mvprintw(5, 5, "Hits: %3d", g->hits);
  mvprintw(6, 5, "Misses: %3d", g->misses);
  mvprintw(7, 5, "Score: %3d", g->score);
  mvprintw(8, 5, "Level: %3d", g->level);
  refresh();
}

void stats(struct game *g)
{
  printf("Hits: %d Misses: %d Score: %d Level: %d\n", g->hits, g->misses, g->score, g->level);
}
