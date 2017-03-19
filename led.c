#include "led.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define LED_PATH "/dev/tty0"

extern int debug;

static int led_fd = -1;
static int led_initcount;

/* led_init - Initialize the led library.

   For each led_init call there should be a corresponding led_uninit
   call.
*/
int led_init(void)
{
  if (led_initcount == 0) {
    led_fd = open(LED_PATH, O_RDONLY);
    if (led_fd == -1) {
      return -1;
    }
  }
  led_initcount++;
  return 0;
}

/* led_uninit - Uninitialize the led library.

   See Also led_init
*/
void led_uninit(void)
{
  if (led_initcount == 0) {
    return;
  }
  if (--led_initcount == 0) {
    close(led_fd);
    led_fd = -1;
  }
}

/* led_status - Return the led status word.

   Each led has a corresponding bit in the status word.  When the led
   is on, the bit is 1; when the led is off, the bit is 0.

   Returns -1 on error.

   See Also led_reset
*/
int led_status(void)
{
  char val;
  if (ioctl(led_fd, KDGETLED, &val) < 0) {
    return -1;
  }
  return val;
}

/* led_reset - Set the led status word.

   See Also led_status
*/
void led_reset(int newstatus)
{
  int i = 1;
  int oldstatus;

  newstatus &= 0xFF;
  ioctl(led_fd, KDSETLED, (char)newstatus);

  while ((oldstatus = led_status()) != newstatus) {
    if (oldstatus == -1) {
      break;
    }
    if (debug && (i % 100) == 0) {
      printf("oldstatus=%x newstatus=%x\n", oldstatus, newstatus);
    }
    i++;
  }
}

/* led_set - Turn on certain leds.

   Keeps the other leds from changing state.

   Returns the previous status word, or -1 on error.
*/
int led_set(int mask, int bit)
{
  int oldstatus = led_status();
  int newstatus;
  if (oldstatus == -1) {
    return -1;
  }
  if (bit) {
    newstatus = oldstatus | mask;
  } else {
    newstatus = oldstatus & ~mask;
  }
  led_reset(newstatus);
  return oldstatus;
}

/* led_setall - Set all leds to either on or off. */
void led_setall(int bit)
{
  led_reset(bit ? (LED_SCR | LED_CAP | LED_NUM) : 0);
}
