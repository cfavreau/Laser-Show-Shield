// Open Laser Show DAC V4B
// By Christopher Favreau
#warning PUT LICENSE HERE!!!
#include <avr/interrupt.h>
#include <avr/io.h>
#include "standard.h"
#include "usart.h"
#include "spi.h"
#include "timer.h"
#include "switch.h"
#include "test_frames.h"

// Port Map
// Jmp   Arduino Pin	AVR Pin		Direction	OLSD Func
//-----------------------------------------------------------------------------
// J1		Digital 0	PD0			Input		Serial RXD (on Arduino)
// J1		Digital 1	PD1			Output		Serial TXD (on Arduino)
// J1		Digital 2	PD2			Output		Status LED (1 == ON)
// J1		Digital 3	PD3(INT1)	Input		User Switch (0 == Pressed)
// J1		Digital 4	PD4			Output		ILDA Shutter
// J1		Digital 7	PD7			Output		#CTS (needs jumper #CTS->FTDI)
// J2		Digital	8	PB0			Output		Latch Color Regs (1 == Latch)
// J2		Digital 9	PB1			Output		#LDAC on uChip DAC
// J2		Digital 10	PB2(SS)		Output		#CS on uChip DAC
// J2		Digital 11	PB3(MOSI)	Output		MOSI (SPI Data Out)
// J2		Digital 13	PB5(SCK)	Output		SCK (SPI Serial Clock)

// Store the Identifier in FLASH
unsigned char OLSD_IDENTIFIER_STRING[] PROGMEM = "OLSD4.1.0\0";

// Status LED Variables
unsigned int status_counter = 0;

// Output Variables
BYTE command = 0;
BYTE x = 0;
BYTE y = 0;
BYTE red = 0;
BYTE green = 0;
BYTE blue = 0;
BYTE fifthbits = 0;

// Laser Packet
BYTE dcommand = 0;
BYTE dx = 0;
BYTE dy = 0;
BYTE dxy = 0;
BYTE drg = 0;
BYTE dbi = 0;

WORD spi_word = 0;

// Pointer to the reset vector for the firmware
void (*reset)( void ) = 0x0000;
void blank_and_center(void);

// ----------------------------------------------------------------------

BOOL output_monochrome_pattern = FALSE;
BOOL output_swap_xy = FALSE;

void output_pattern(BYTE *pStream, unsigned int length)
{
	unsigned int i = 0;

	while (i < length)
	{
		// Read a command
		command = pgm_read_byte(&pStream[i++]);

		// This being a test frame
		// We should... have a point output command
		if ((command != 0x20) && (command != 0x21))
		{
			// hmmm... we had some other command
			// Just get the next byte in the streams
			continue;
		}

		// Read the X and Y coordinates from the stream
		dx = pgm_read_byte(&pStream[i++]);
		dy = pgm_read_byte(&pStream[i++]);
		dxy = pgm_read_byte(&pStream[i++]);
		
		// Check to see if the colors stay the same
		// If they do stay the same then don't load the color bytes
		if (command != 0x21)
		{
			// Read the color bytes
			drg = pgm_read_byte(&pStream[i++]);
			dbi = pgm_read_byte(&pStream[i++]);
		}
		
		// Output our point
		// Now output the actual point
		
		// Decode into 4 bit RGBI
		red = (drg >> 4);
		green = (drg & 0x0F);
		blue = (dbi >> 4);
		
		// Check the monochrome flag
		if (output_monochrome_pattern)
		{
			// Make all the colors the same brightness
			register unsigned char color = red;
			if (green > color) color = green;
			if (blue > color) color = blue;
			red = color;
			green = color;
			blue = color;
		}

		// Do the fifth bit for the color
		fifthbits = 0;
		if (red) fifthbits |= 0x10;
		if (green) fifthbits |= 0x20;
		if (blue) fifthbits |= 0x40;

		// Check to see if it is time to output a point yet?
		while (!output_flag) asm("nop");
		// Reset our output flag/counter
		output_flag = 0;

		// Pull up the latch lines on the DAC and colors before we
		// go and send more data down the SPI bus
		// LDAC
		portb.bit1 = 1;
		// Color Latches
		portb.bit0 = 0;

		// Load DAC A
		// X X X PowerMode Load Output AddrB AddrA 
		// pull down chip select!
		if (!output_swap_xy)
			spi_word = 0b0111000000000000 | (dy << 4) | ((dxy & 0xF0) >> 4);
		else
			spi_word = 0b0111000000000000 |(dx << 4) | (dxy & 0x0F);
		portb.bit2 = 0;
		SPDR = spi_word >> 8;
		while (!(SPSR & (1 << SPIF)));
		SPDR = spi_word & 0x00FF;
		while (!(SPSR & (1 << SPIF)));
		// pull up chip select!
		portb.bit2 = 1;
		asm("nop");
		
		// NOP NOP NOP NOP!??? how many?
		// Load DAC B and update the output (assign a 0 to output)
		// pull down chip select!
		if (!output_swap_xy)
			spi_word = 0b1111000000000000 | (dx << 4) | (dxy & 0x0F);
		else
			spi_word = 0b1111000000000000 | (dy << 4) | ((dxy & 0xF0) >> 4);
		portb.bit2 = 0;
		SPDR = spi_word >> 8;
		while (!(SPSR & (1 << SPIF)));
		SPDR = spi_word & 0x00FF;
		while (!(SPSR & (1 << SPIF)));
		// Pull up chip select!
		portb.bit2 = 1;

		// Colors
		// *** REMEMBER that a shift register passes the 1st 8 bits
		// on to the 2nd shift register so we do the BLUE first!
		// B goes first
		SPDR = fifthbits | blue;
		while (!(SPSR & (1 << SPIF)));
		
		// RG go second
		SPDR = (green << 4) | red;
		while (!(SPSR & (1 << SPIF)));
		
		// Now latch the Colors and the DAC
		portb.bit0 = 1;

		// Pull the LDAC line LOW to load the DAC's values
		portb.bit1 = 0;

		// and set the shutter to OFF since it might be ON
		portd.bit4 = 0;

		// Do the Status LED
		status_counter++;
		if (status_counter < 500)
		{
			// Reset the LED to flip
			portd.bit2 = 1;
		}
		else
		{
			portd.bit2 = 0;
		}
		if (status_counter > 1000) status_counter = 0;
	}
}

void output_test_pattern(void)
{
	// Clear the button press
	switch_pressed = 0;

	// Reset the output monochrome flag
	output_monochrome_pattern = FALSE;

	// The first button press starts the ILDA 12k Test Pattern
	// Then:  Square Wave, Laser Media, New Test Pattern
	// This Round: 12 kpps
	// Next Round: 30 kpps

	while (1)
	{
		switch ((switch_pressed % 13) + 1)
		{
		case (1):
			set_timer_pps(12000);
			output_pattern(ILDA12K_IMAGE, ILDA12K_POINT_COUNT);
			break;
		case (2):
			set_timer_pps(12000);
			output_pattern(SQUARE_IMAGE, SQUARE_POINT_COUNT);
			break;
		case (3):
			set_timer_pps(12000);
			output_pattern(LASERMEDIA_IMAGE, LASERMEDIA_POINT_COUNT);
			break;
		case (4):
			set_timer_pps(12000);
			output_pattern(NEWTEST_IMAGE, NEWTEST_POINT_COUNT);
			break;
		case (5):
			set_timer_pps(12000);
			output_swap_xy = TRUE;
			output_pattern(NEWTEST_IMAGE, NEWTEST_POINT_COUNT);
			output_swap_xy = FALSE;
			break;
		case (6):
			blank_and_center();
			// We are already blanked... just wait for the switch
			wait_for_switch();
			break;
		case (7):
			set_timer_pps(30000);
			output_pattern(ILDA12K_IMAGE, ILDA12K_POINT_COUNT);
			break;
		case (8):
			set_timer_pps(30000);
			output_pattern(SQUARE_IMAGE, SQUARE_POINT_COUNT);
			break;
		case (9):
			set_timer_pps(30000);
			output_pattern(LASERMEDIA_IMAGE, LASERMEDIA_POINT_COUNT);
			break;
		case (10):
			set_timer_pps(30000);
			output_pattern(NEWTEST_IMAGE, NEWTEST_POINT_COUNT);
			break;
		case (11):
			set_timer_pps(30000);
			output_swap_xy = TRUE;
			output_pattern(NEWTEST_IMAGE, NEWTEST_POINT_COUNT);
			output_swap_xy = FALSE;
			break;
		case (12):
			blank_and_center();
			// Toggle the monochrome flag
			output_monochrome_pattern = !output_monochrome_pattern;
			// It is important to block so we don't toggle this flag
			// randomly ... wait for the switch again
			wait_for_switch();
			break;
		}
	}
}

inline void PrintIdentifier(void)
{
	// Get the OLSD Identifier from Flash instead of RAM
	// so we don't use up valuable RAM
	unsigned char i = 0;
	unsigned char byte = pgm_read_byte(&OLSD_IDENTIFIER_STRING[i]);
	// Loop until we run into the NULL terminator
	while (byte)
	{
		USART_Write(byte);
		byte = pgm_read_byte(&OLSD_IDENTIFIER_STRING[i++]);
	}
}

void blank_and_center(void)
{
	// Define a center
	#define CENTER_X 2047
	#define CENTER_Y 2047

	// Pull up the latch lines on the DAC and colors before we
	// go and send more data down the SPI bus
	// LDAC
	portb.bit1 = 1;
	// Color Latches
	portb.bit0 = 0;

	// Load DAC A
	// X X X PowerMode Load Output AddrB AddrA 
	// pull down chip select!
	spi_word = 0b0111000000000000 | CENTER_X;
	portb.bit2 = 0;
	SPDR = spi_word >> 8;
	while (!(SPSR & (1 << SPIF)));
	SPDR = spi_word & 0x00FF;
	while (!(SPSR & (1 << SPIF)));
	// pull up chip select!
	portb.bit2 = 1;
	asm("nop");
	
	// NOP NOP NOP NOP!??? how many?
	// Load DAC B and update the output (assign a 0 to output)
	// pull down chip select!
	spi_word = 0b1111000000000000 | CENTER_Y;
	portb.bit2 = 0;
	SPDR = spi_word >> 8;
	while (!(SPSR & (1 << SPIF)));
	SPDR = spi_word & 0x00FF;
	while (!(SPSR & (1 << SPIF)));
	// Pull up chip select!
	portb.bit2 = 1;

	// Colors
	// RG go first
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	
	// B (and the 5th bits) go next
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	
	// Now latch the Colors and the DAC
	portb.bit0 = 1;

	// Pull the LDAC line LOW to load the DAC's values
	portb.bit1 = 0;

	// and set the shutter to closed!!!
	portd.bit4 = 1;

	// Wait a few cycles before setting the latch lines up again
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	// Pull up the latch lines on the DAC and colors before we
	// go and send more data down the SPI bus
	// LDAC
	portb.bit1 = 1;
	// Color Latches
	portb.bit0 = 0;
}

// DEBUG
unsigned char cx = 0;
unsigned char cy = 0;

int main(void)
{
	// Initialize our IO Ports
	
	// Port D - Serial + Status LED + User Switch + Shutter + CTS
	DDRD = 0b10010100;
	//PORTD = 0b00000011;
	// Pull up the user switch line
	ddrd.bit3 = 0;
	portd.bit3 = 1;

	/*
	// Test LED and Switch CODE
	while (1)
	{
		portd.bit2 = !pind.bit3;
	}
	*/

	// Initialize our SPI port to output to our DAC
	ddrb.bit2 = 1;
	portb.bit2 = 1;
	SPI_Init();

	// LDAC - disable
	ddrb.bit1 = 1;
	portb.bit1 = 1;

	// CTS Pin
	ddrb.bit0 = 1;
	portb.bit0 = 0;

	// Status LED
	ddrc.bit4 = 1;
	// Turn the LED ON
	portd.bit2 = 1;

	// Initialize our USART at 2 Mbps
	USART_Init();

	// Initialize the output point timer
	init_timer();

	// Blank and center
	blank_and_center();

	// Initialize the button input device
	init_switch();

	// Turn on interrupts
	sei();

	// Print Boot Message
	PrintIdentifier();

	CTS_ON;

	// DEBUG
	//output_test_pattern();
	//while (1) output_pattern(NEWTEST_IMAGE, NEWTEST_POINT_COUNT);

	// Loop forever
	while (1)
	{
		// Tell the USB chip we want to start getting data over our serial port
		CTS_ON;

		// DEBUG
		//switch_pressed = 1;
		
		// Read the command byte from the packet
		//READ(command);
		while (!(UCSR0A & (1 << RXC0)) && (switch_pressed == 0)); dcommand = UDR0;
		//while (!(UCSR0A & (1 << RXC0))); dcommand = UDR0;
		//READ(dcommand);

		///*
		// Before we read a command packet
		// We need to check to see if the button was pressed.
		// If the button was pressed then go into the Test Output Routine
		if (switch_pressed > 0)
		{
			// Jump to the test pattern routine
			output_test_pattern();
			// If we ever return...
			// Rejoin the loop after we are done with the test pattern
			continue;
		}
		//*/

		// Extract the command from the dcommand byte
		command = dcommand & 0xF0;

		// Check to see if we have a point output command
		// Command 0x20 -> Output Point
		if (command != 0x20)
		{
			// Check to see if it is a synchronization command
			if (command == 0xFF) continue;
			
			// Read the rest of the packet
			READ(dx);
			READ(dy);
			READ(dxy);
			READ(drg);

			// Check to see if it is a point output rate command
			if (command == 0x10)
			{
				// Set the point output rate
				// dx is the upper byte and dy is the lower byte
				// of the 16 bit unsigned pps rate
				set_timer_pps((dx << 8) + dy);
			}
			else
			// Check to see if it is a Reset command
			if (command == 0xF0)
			{
				// Reset the Firmware
				reset();
			}
			else
			// Check to see if it is Identify command
			if (command == 0xE0)
			{
				// Send the identifier string
				PrintIdentifier();
			}
			else
			{
				// What was that
				USART_Write('?');
			}

			// We aren't going to output a point
			continue;
		}

		// Command 0x20 - point output
		// Read the XY and colors
		READ(dx);
		READ(dy);
		READ(dxy);
		READ(drg);
		
		// Tell the USB chip we want to STOP getting data over our serial port
		CTS_OFF;
		
		// Output our point
		
		// Decode into 4 bit RGBI
		red = (drg >> 4);
		green = (drg & 0x0F);
		blue = (dcommand & 0x0F);
		
		// Fifth bits are the UPPER Nibble
		fifthbits = 0;
		if (red) fifthbits |= 0x10;
		if (green) fifthbits |= 0x20;
		if (blue) fifthbits |= 0x40;

		// Check to see if it is time to output a point yet?
		while (!output_flag) asm("nop");
		// Reset our output flag/counter
		output_flag = 0;

		// Pull up the latch lines on the DAC and colors before we
		// go and send more data down the SPI bus
		// LDAC
		portb.bit1 = 1;
		// Color Latches
		portb.bit0 = 0;

		// Load DAC A
		// X X X PowerMode Load Output AddrB AddrA 
		// pull down chip select!
		spi_word = 0b0111000000000000 | (dy << 4) | ((dxy & 0xF0) >> 4);
		portb.bit2 = 0;
		SPDR = spi_word >> 8;
		while (!(SPSR & (1 << SPIF)));
		SPDR = spi_word & 0x00FF;
		while (!(SPSR & (1 << SPIF)));
		// pull up chip select!
		portb.bit2 = 1;
		asm("nop");
		
		// NOP NOP NOP NOP!??? how many?
		// Load DAC B and update the output (assign a 0 to output)
		// pull down chip select!
		spi_word = 0b1111000000000000 | (dx << 4) | (dxy & 0x0F);
		portb.bit2 = 0;
		SPDR = spi_word >> 8;
		while (!(SPSR & (1 << SPIF)));
		SPDR = spi_word & 0x00FF;
		while (!(SPSR & (1 << SPIF)));
		// Pull up chip select!
		portb.bit2 = 1;

		// Colors
		// *** REMEMBER that a shift register passes the 1st 8 bits
		// on to the 2nd shift register so we do the BLUE first!
		// B goes first
		SPDR = fifthbits | blue;
		while (!(SPSR & (1 << SPIF)));

		// RG go second
		SPDR = (green << 4) | red;
		while (!(SPSR & (1 << SPIF)));
		
		// Now latch the Colors and the DAC
		portb.bit0 = 1;

		// Pull the LDAC line LOW to load the DAC's values
		portb.bit1 = 0;

		// and set the shutter to OFF since it might be ON
		portd.bit4 = 0;

		// Do the Status LED
		status_counter++;
		if (status_counter < 500)
		{
			// Reset the LED to flip
			portd.bit2 = 1;
		}
		else
		{
			portd.bit2 = 0;
		}
		if (status_counter > 1000) status_counter = 0;
	}

	return 1;
}

