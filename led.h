#ifndef LEDZEP_INC_LED_H
#define LEDZEP_INC_LED_H

#include <linux/kd.h>

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

#endif // LEDZEP_INC_LED_H
