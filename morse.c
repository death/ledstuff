/*
 * morse - Useful to avoid Van Eck phreaking, rampant in Philippine
 * jails.
 */
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include "common.h"
#include "led.h"

enum {
      /* My keyboard's leds are too slow for 50ms. */
      DOTLENGTH = 100000
};

/* These are for ITU morse code. */

const char *alphamorse[] =
  {
   ".-", "-...", "-.-.", "-..", ".",
   "..-.", "--.", "....", "..", ".---",
   "-.-", ".-..", "--", "-.", "---",
   ".--.", "--.-", ".-.", "...", "-",
   "..-", "...-", ".--", "-..-", "-.--",
   "--.."
  };

const char *nummorse[] =
  {
   "-----", ".----", "..---", "...--", "....-",
   ".....", "-....", "--...", "---..", "----."
  };

void send(const char *letter, int wordboundary);
void dotsleep(int units);

int main(void)
{
  int wordboundary = 0;
  int c;

  ledapp_setup();

  while ((c = toupper(fgetc(stdin))) != EOF) {
    if (c >= 'A' && c <= 'Z') {
      send(alphamorse[c - 'A'], wordboundary);
      wordboundary = 0;
    } else if (c >= '0' && c <= '9') {
      send(nummorse[c - '0'], wordboundary);
      wordboundary = 0;
    } else {
      wordboundary = 1;
    }
  }

  ledapp_teardown();
}

void send(const char *letter, int wordboundary)
{
  int i;

  if (wordboundary) {
    /* The space between words is seven units; we take into account
       the previously written letter. */
    dotsleep(4);
  }

  for (i = 0; letter[i]; i++) {
    led_set(LED_SCR, 1);
    dotsleep(letter[i] == '.' ? 1 : 3);
    led_set(LED_SCR, 0);
    dotsleep(1);
  }

  /* The space between letters is three units; we take into account
     the previous letter part. */
  dotsleep(2);
}

void dotsleep(int units)
{
  usleep(DOTLENGTH * units);
}
