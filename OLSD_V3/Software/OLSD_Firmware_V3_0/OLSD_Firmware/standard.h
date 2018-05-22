// Standard Types & Constant Definitions

#ifndef __STANDARD_H__
#define __STANDARD_H__

// Standard Definitions
#define TRUE	1
#define	FALSE	0
#define NULL	0
#define CR		13
#define LF		10
#define BOOL	unsigned char
#define BYTE	unsigned char
#define WORD	unsigned int

// Bit Field Conversion for PORT Access
typedef struct
{
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
} BYTE_BITFIELD;

// Data Direction
#define ddra (*((volatile BYTE_BITFIELD*)(&DDRA)))
#define ddrb (*((volatile BYTE_BITFIELD*)(&DDRB)))
#define ddrc (*((volatile BYTE_BITFIELD*)(&DDRC)))
#define ddrd (*((volatile BYTE_BITFIELD*)(&DDRD)))

// Output Port
#define porta (*((volatile BYTE_BITFIELD*)(&PORTA)))
#define portb (*((volatile BYTE_BITFIELD*)(&PORTB)))
#define portc (*((volatile BYTE_BITFIELD*)(&PORTC)))
#define portd (*((volatile BYTE_BITFIELD*)(&PORTD)))

// Input Port
#define pina (*((volatile BYTE_BITFIELD*)(&PINA)))
#define pinb (*((volatile BYTE_BITFIELD*)(&PINB)))
#define pinc (*((volatile BYTE_BITFIELD*)(&PINC)))
#define pind (*((volatile BYTE_BITFIELD*)(&PIND)))

#endif	// __STANDARD_H__
