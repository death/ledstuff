/*
 * travel - The result of watching too much Star Trek and playing with
 * keyboard leds.
 *
 * "One cannot watch \"too much\" Star Trek."
 *             -- Mr. Spock, stardate unknown
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "common.h"
#include "led.h"

enum {
      DURATION = 6000000,
      TRAVELS = 10,
      NSTEPS = 3 * TRAVELS + 1,
      LINGER = 3 * TRAVELS,
      MINSPEED = 40000,
      MAXSPEED = 1000000
};

int stepnum;
int wait;
int led;

void start(void);
void step(void);
void linger(void);
int keepgoing(void);

int main(void)
{
  ledapp_setup();

  start();
  while (keepgoing()) {
    step();
  }
  linger();

  ledapp_teardown();
}

void start(void)
{
  stepnum = 1;
  led = LED_SCR;
  led_setall(0);
}

static void ledstep(void)
{
  /* Rotate led bits left by 1. */
  led = ((led << 1) | (led >> 2)) & 7;
  led_reset(led);
}

void step(void)
{
  ledstep();

  /* Perform exponential out-easing. */
  int next = (int)(DURATION * (-pow(2, -5 * (double)min(stepnum, NSTEPS) / NSTEPS) + 1));
  wait = clamp((next - elapsed()) | 1, MINSPEED, MAXSPEED);
  usleep(wait);

  stepnum++;
}

void linger(void)
{
  int i;
  for (i = 0; i < LINGER; i++) {
    ledstep();
    usleep(wait);
  }
}

int keepgoing(void)
{
  return stepnum < NSTEPS;
}
