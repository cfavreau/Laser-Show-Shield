#include <avr/interrupt.h>		// For Interrupt Handling
#include "standard.h"			// Standard Types & Definitions

#include "timer.h"				// Timer Function Prototypes & Definitions
#include "spi.h"

#define MIN_PPS_RATE		1000
#define MAX_PPS_RATE		30000
#define DEFAULT_PPS			12000

#define TIMER_FUDGE_FACTOR	128
#define DEFAULT_TIMEOUT  TIMER_FUDGE_FACTOR + 65002 //64202	// 12 kpps

volatile BYTE output_flag = FALSE;

// Timer Count Register Reset Value
volatile unsigned int timer_start_value = 0;

void init_timer(void)
{
	// Intialize the Timer1

	// Stop the timer
	TCCR1A = 0x00;
	TCCR1B = 0x00;

	// Set the initial timeout value to the default output rate
	set_timer_pps(DEFAULT_PPS);

	// Configure the Timer1 Overflow Interrupt to be ON
	TIMSK1 = 0b00000001;

	// Select the IO clock with NO prescalar
	TCCR1B = 0x01;
}

void set_timer_pps(unsigned int pps)
{
	// Bounds check the points per second
	if (pps > MAX_PPS_RATE) pps = MAX_PPS_RATE;
	if (pps < MIN_PPS_RATE) pps = MIN_PPS_RATE;

	// Convert points per second to thousands of points per second
	pps /= 1000;

	// Calculate the timer timeout value from the
	// number of points per second we are supposed
	// to display
	// Timer ceiling - (Clock Rate in kHZ / PPS) + Fudge Factor (found with scope)
	timer_start_value = 0xFFFF - (16000 / pps) + TIMER_FUDGE_FACTOR;

	// Set the timer count register
	TCNT1 = timer_start_value;
}

ISR(TIMER1_OVF_vect)
{
	// Reset the counter
	TCNT1 = timer_start_value;

	// Check to see if the output flag has been reset
	if (output_flag > 5)
	{
		// BLANK!!!!
		portb.bit0 = 0;
		SPDR = 0;
		while (!(SPSR & (1 << SPIF)));
		// RG go second
		SPDR = 0;
		while (!(SPSR & (1 << SPIF)));
		// Now latch the Colors and the DAC
		portb.bit0 = 1;

		// and set the shutter to ON
		portd.bit4 = 1;

		// Turn the LED ON
		portd.bit2 = 1;
	}
	
	// Output a point NOW!
	output_flag++;
}
