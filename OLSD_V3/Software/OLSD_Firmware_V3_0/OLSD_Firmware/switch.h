// Switch Functions

#ifndef __SWITCH_H__
#define __SWITCH_H__

extern volatile unsigned char switch_pressed;
extern volatile unsigned char debounce_wait;

void init_switch(void);
void wait_for_switch(void);

#endif // __TIMER_H__
