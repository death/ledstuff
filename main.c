#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include "led.h"

enum {
      DURATION = 6000000,
      TRAVELS = 10,
      NSTEPS = 3 * TRAVELS + 1,
      LINGER = 3 * TRAVELS,
      MINSPEED = 40000,
      MAXSPEED = 1000000
};

enum {
      EASE_LINEAR,
      EASE_IN_QUAD,
      EASE_OUT_QUAD,
      EASE_INOUT_QUAD,
      EASE_IN_CUBIC,
      EASE_OUT_CUBIC,
      EASE_INOUT_CUBIC,
      EASE_IN_SINE,
      EASE_OUT_SINE,
      EASE_INOUT_SINE,
      EASE_IN_EXP,
      EASE_OUT_EXP,
      EASE_INOUT_EXP,
      EASE_IN_CIRC,
      EASE_OUT_CIRC,
      EASE_N,

      EASE_DEFAULT = EASE_OUT_EXP
};

static int debug;
static int mode = EASE_DEFAULT;
static int stepnum;
static int elapsed;
static int wait;
static int led;

void cleanup(int);
void start(void);
void step(void);
void linger(void);
void final(void);
int keepgoing(void);
int ease(int start, int end, int elapsed, int duration);

int main(int argc, char *argv[])
{
  if (led_init() < 0) {
    fprintf(stderr, "can't initialize led library; did you run as superuser?\n");
    return 1;
  }

  if (argc > 1) {
    mode = atoi(argv[1]);
    if (mode < 0 || mode > EASE_N) {
      mode = EASE_DEFAULT;
    }
  }

  signal(SIGINT, cleanup);

  start();
  while (keepgoing()) {
    step();
  }
  linger();

  signal(SIGINT, SIG_DFL);
  cleanup(0);
}

void cleanup(int s)
{
  s;
  final();
  led_uninit();
  exit(0);
}

void start(void)
{
  elapsed = 0;
  stepnum = 1;
  led = LED_SCR;
}

static void ledstep(void)
{
  switch (led) {
  default:
  case LED_SCR:
    led = LED_NUM;
    break;
  case LED_CAP:
    led = LED_SCR;
    break;
  case LED_NUM:
    led = LED_CAP;
    break;
  }
  led_reset(led);
}

static int max(int a, int b)
{
  return a < b ? b : a;
}

static int min(int a, int b)
{
  return a < b ? a : b;
}

void step(void)
{
  ledstep();

  int next = ease(0, DURATION, stepnum, NSTEPS);
  wait = (next - elapsed) | 1;
  wait = min(max(wait, MINSPEED), MAXSPEED);
  usleep(wait);
  elapsed += wait;
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

void final(void)
{
  led_setall(0);
}

int keepgoing(void)
{
  return stepnum < NSTEPS;
}

int ease(int start, int end, int step, int nsteps)
{
  int result;
  int d = end - start;
  double x;
  double f;

  if (step > nsteps) {
    step = nsteps;
  }
  x = (double)step / nsteps;

  switch (mode) {
  default:
  case EASE_LINEAR:
    f = x;
    break;
  case EASE_IN_QUAD:
    f = x*x;
    break;
  case EASE_OUT_QUAD:
    f = -1*x*(x-2);
    break;
  case EASE_INOUT_QUAD:
    x *= 2;
    if (x < 1.0) {
      f = 0.5*x*x;
    } else {
      x--;
      f = -0.5*(x*(x-2)-1);
    }
    break;
  case EASE_IN_CUBIC:
    f = x*x*x;
    break;
  case EASE_OUT_CUBIC:
    x--;
    f = x*x*x+1;
    break;
  case EASE_INOUT_CUBIC:
    x *= 2;
    if (x < 1) {
      f = 0.5*x*x*x;
    } else {
      x -= 2;
      f = 0.5*(x*x*x + 2);
    }
    break;
  case EASE_IN_SINE:
    f = -1*cos(x*3.141592*0.5)+1;
    break;
  case EASE_OUT_SINE:
    f = sin(x*3.141592*0.5);
    break;
  case EASE_INOUT_SINE:
    f = -0.5 * (cos(x * 3.141592) - 1);
    break;
  case EASE_IN_EXP:
    f = pow(2, 5*(x-1));
    break;
  case EASE_OUT_EXP:
    f = -pow(2, -5*x)+1;
    break;
  case EASE_INOUT_EXP:
    x *= 2;
    if (x < 1) {
      f = 0.5*pow(2, 5*(x-1));
    } else {
      x--;
      f = 0.5*-pow(2, -5*x)+1;
    }
    break;
  case EASE_IN_CIRC:
    f = -1*(sqrt(1-(x*x))-1);
    break;
  case EASE_OUT_CIRC:
    x--;
    f = sqrt(1-x*x);
    break;
  }

  result = (int)(start + d * f);
  if (debug) {
    printf("start=%d end=%d step=%d nsteps=%d result=%d\n", start, end, step, nsteps, result);
  }
  return result;
}
