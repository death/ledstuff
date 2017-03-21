#ifndef LEDZEP_INC_COMMON_H
#define LEDZEP_INC_COMMON_H

void ledapp_setup(void);
void ledapp_teardown(void);

int max(int a, int b);
int min(int a, int b);
int clamp(int v, int lo, int up);
int elapsed(void);

#endif // LEDZEP_INC_COMMON_H
