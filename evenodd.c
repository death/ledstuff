/*
   TODO

   * Show elapsed time, wins, and score.

   * Show statistics at the end of a game.

   * Allow control of halfdelay value (levels?).
 */
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>

#include "ledapp.h"
#include "led.h"

enum {
      HALFDELAY = 10
};

int main(void)
{
  int lose = 0;
  int wins = 0;
  int score = 0;

  ledapp_setup();

  setlocale(LC_ALL, "");
  initscr();
  noecho();
  halfdelay(HALFDELAY);

  while (!lose) {
    int problem = 0;
    int on = 0;

    if (rand() & 1) {
      problem |= LED_NUM;
      on++;
    }
    if (rand() & 1) {
      problem |= LED_SCR;
      on++;
    }
    if (rand() & 1) {
      problem |= LED_CAP;
      on++;
    }

    led_reset(problem);

    switch (getch()) {
    case 'e':
      if (on & 1) {
        lose = 1;
      } else {
        wins++;
        score++;
      }
      break;
    case 'o':
      if (on & 1) {
        wins++;
        score++;
      } else {
        lose = 1;
      }
      break;
    case ERR:
      if (score > 0) {
        score--;
      }
    default:
      break;
    }
  }

  endwin();
  ledapp_teardown();

  return 0;
}
