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

static int led_fd = -1;
static int led_initcount = 0;
static int led_stackptr = 0;
static int led_stack[LED_STACK_SIZE];
static FILE *led_traceoutput = 0;

#define LED_TRACE(...)                                  \
  do {                                                  \
    if (led_traceoutput) {                              \
      fprintf(led_traceoutput, __VA_ARGS__);            \
    }                                                   \
  } while (0)

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
  if (ioctl(led_fd, KDSETLED, (char)newstatus) < 0) {
    LED_TRACE("KDSETLED ioctl returned -1\n");
  }

  while ((oldstatus = led_status()) != newstatus) {
    if (oldstatus == -1) {
      LED_TRACE("led_status returned -1 in led_reset\n");
      break;
    }
    if ((i & 0x80) == 0) {
      LED_TRACE("oldstatus=%x newstatus=%x\n", oldstatus, newstatus);
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

/* led_push - Push current status word into led stack.

   Return -1 on error, and the number of elements in the stack on
   success.
*/
int led_push(void)
{
  if (led_stackptr == LED_STACK_SIZE) {
    return -1;
  }
  led_stack[led_stackptr] = led_status();
  LED_TRACE("push led_stack[%d] = %x\n", led_stackptr, led_stack[led_stackptr]);
  if (led_stack[led_stackptr] == -1) {
    return -1;
  }
  led_stackptr++;
  return led_stackptr;
}

/* led_pop - Pop current status word from led stack.

   Does nothing if stack is empty.
*/
void led_pop(void)
{
  if (led_stackptr == 0) {
    return;
  }
  led_stackptr--;
  LED_TRACE("pop led_stack[%d] = %x\n", led_stackptr, led_stack[led_stackptr]);
  led_reset(led_stack[led_stackptr]);
}

/* led_trace - Set a trace output stream.

   Disables tracing if NULL is passed.
 */
void led_trace(FILE *stream)
{
  led_traceoutput = stream;
}
