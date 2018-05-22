// Timer Functions

#ifndef __TIMER_H__
#define __TIMER_H__

extern volatile BYTE output_flag;

void init_timer(void);

void set_timer_pps(unsigned int pps);

#endif // __TIMER_H__
