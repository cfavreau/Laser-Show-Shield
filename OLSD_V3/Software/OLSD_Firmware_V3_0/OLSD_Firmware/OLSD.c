#include <avr/interrupt.h>
#include <avr/io.h>
#include "standard.h"
#include "usart.h"
#include "spi.h"
#include "timer.h"
#include "switch.h"
#include "test_frames.h"

// Port Map
// Control
// PORTB0 = #CTS

// 12 Bit DAC
// PORTB1 = #LDAC
// PORTB2 = #SS
// PORTB3 = MOSI
// PORTB5 = SCK
// 
// Serial Port
// PORTD0 = ??
// PORTD1 = ??

// RED
// PORTD5 = Red0
// PORTD6 = Red1
// PORTD7 = Red3
//
// GREEN
// PORTD2 = Green0
// PORTD3 = Green1
// PORTD4 = Green2
//
// BLUE
// PORTC0 = Blue0
// PORTC1 = Blue1
// PORTC2 = Blue2
// PORTC3 = Shutter
// PORTC4 = Status LED
// PORTC5 = Momentary Switch

// Store the Identifier in FLASH
unsigned char OLSD_IDENTIFIER_STRING[] PROGMEM = "OLSD1.0.0\0";

// Status LED Variables
unsigned int iStatusCounter = 500;
BOOL bStatusLED = TRUE;

// Output Variables
BYTE command = 0;
BYTE x = 0;
BYTE y = 0;
BYTE red = 0;
BYTE green = 0;
BYTE blue = 0;

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
		
		// Decode into 3 bit RGBI
		// TODO - > use Intensity (or the 4th color)
		red = (drg >> 5);
		green = (drg & 0x0F) >> 1;
		blue = (dbi >> 5);
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
		
		// Check to see if it is time to output a point yet?
		while (!output_flag) asm("nop");
		// Reset our output flag/counter
		output_flag = 0;

		portb.bit1 = 1;

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

		// Red = 0
		// Green = 0
		PORTD = 0b00000011 | (red << 5) | (green << 2);

		// Blue = 0
		// Shutter = 1
		PORTC = 0b00111000 | (blue & 0x07);

		// Pull the LDAC line LOW to load the DAC's values
		portb.bit1 = 0;

		// Do the Status LED
		if (iStatusCounter-- < 1)
		{
			// Reset the LED to flip when we
			// Send 500 points
			iStatusCounter = 500;
			bStatusLED = !bStatusLED;
		}
		portc.bit4 = bStatusLED;
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

	portb.bit1 = 1;

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
	portb.bit1 = 0;

	// Red = 0
	// Green = 0
	PORTD = 0b00000011;
	// Blue = 0
	// Shutter = 1
	PORTC = 0b00111000;
}

int main(void)
{
	// Initialize our IO Ports
	// Port C = Button + LED + Shutter + Blue
	DDRC = 0b00011111;
	PORTC = 0b00111000;
	
	// Port D - Serial + Red and Green
	DDRD = 0b11111100;
	PORTD = 0b00000011;

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
	portc.bit4 = 1;		// Turn the LED ON

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
			//READ(dbi);

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
		
		// Decode into 3 bit RGBI
		// Red and Green are stored together
		red = (drg >> 5);
		green = (drg & 0x0F) >> 1;
		// Blue is stored in the lower nibble of the command byte
		blue = (dcommand & 0x0F) >> 1;
		
		// Check to see if it is time to output a point yet?
		while (!output_flag) asm("nop");
		// Reset our output flag/counter
		output_flag = 0;

		portb.bit1 = 1;

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

		// Red = 0
		// Green = 0
		PORTD = 0b00000011 | (red << 5) | (green << 2);

		// Blue = 0
		// Shutter = 0
		PORTC = 0b00110000 | blue;

		// Pull the LDAC line LOW to load the DAC's values
		portb.bit1 = 0;

		// Do the Status LED
		if (iStatusCounter-- < 1)
		{
			// Reset the LED to flip when we
			// Send 500 points
			iStatusCounter = 500;
			bStatusLED = !bStatusLED;
		}
		portc.bit4 = bStatusLED;
	}

	return 1;
}

