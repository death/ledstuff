#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "ledapp.h"
#include "led.h"

static void cleanup(int s)
{
  (void)s;
  led_setall(0);
  led_uninit();
  exit(0);
}

void ledapp_setup(void)
{
  if (led_init() < 0) {
    fprintf(stderr, "can't initialize led library; are you running as superuser?\n");
    exit(1);
  }

  /* We don't push/pop because, frankly, the led status word is not
     reflective of the actual led states if the user interferes by,
     say, pressing one of the corresponding keys. */
  led_setall(0);

  signal(SIGINT, cleanup);
}

void ledapp_teardown(void)
{
  signal(SIGINT, SIG_DFL);
  cleanup(0);
}
