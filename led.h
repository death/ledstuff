#ifndef LEDSTUFF_INC_LED_H
#define LEDSTUFF_INC_LED_H

#include <stdio.h>
#include <linux/kd.h>

/*
 * In our programs, we may assume that the led constants (LED_CAP and
 * friends) have the values given in console_ioctl(4) manpage
 */

enum {
      LED_STACK_SIZE = 16
};

int led_init(void);
void led_uninit(void);
int led_status(void);
void led_reset(int newstatus);
int led_set(int mask, int bit);
void led_setall(int bit);
int led_push(void);
void led_pop(void);
void led_trace(FILE *stream);

#endif // LEDSTUFF_INC_LED_H
