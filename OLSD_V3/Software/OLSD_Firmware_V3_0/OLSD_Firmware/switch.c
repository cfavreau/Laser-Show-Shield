#include <avr/interrupt.h>
#include <avr/io.h>
#include "standard.h"

volatile unsigned char switch_pressed = 0;
volatile unsigned char switch_debounce_wait = 0;

void init_switch(void)
{
	// Setup the button pressed interrupt
	// The button is on PORTC 5
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT13);
	
	// Trigger on a rising edge
	EICRA = 0b00000100;
}

void wait_for_switch(void)
{
	// Wait for the switch to be pressed again
	unsigned char temp = switch_pressed;
	while (temp == switch_pressed) asm("nop");
}

ISR(TIMER2_OVF_vect)
{
	// Reset the count
	TCNT2 = 0;
	
	// Count down... a count of 61 is approx. 1 second
	// We want to wait 1/5 of 1 second... so count to 20
	switch_debounce_wait--;
	
	// Check to see if we have finished the wait
	if (switch_debounce_wait == 0)
	{
		// Turn off the timer
		TCCR2B = 0;
	}
}

#define SWITCH_DEBOUNCE_COUNT	15

ISR(PCINT1_vect)
{
	if ((pinc.bit5 == 0) && (switch_debounce_wait == 0))
	{
		// Set the debounce wait count to 20 (~1/5 second)
		switch_debounce_wait = SWITCH_DEBOUNCE_COUNT;

		// Start the debounce delay timer
		TCNT2 = 0;
		TIMSK2 = (1 << TOIE2);
		TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
		
		// Increment the button pressed count
		switch_pressed++;

		// Feed the USART register with 0xFF (do nothing)
		// In case we are waiting for a Command
		UDR0 = 0xFF;

		// Turn the LED OFF - to signify the button press
		// was received
		portc.bit4 = 0;
	}
}
