#include <avr/interrupt.h>
#include "standard.h"
#include <avr/io.h>
#include "usart.h"

// *********************************************************************************
// usart.c
// *********************************************************************************

void USART_Init(void)
{
	// Set Baud Rate to 2 Mbps (16 MHz Clock Rate)
	UBRR0H = 0;
	UBRR0L = 0;			// From PG. 199 of the ATMega328s Manual
	//UBRR0L = 3;				// 500 kbps

	// High Speed Mode ON
	UCSR0A = (1 << U2X0);
	
	// Enable transmit and receive (no interrupts)
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void USART_Write(unsigned char data)
{
	// Wait Until The Transmit Buffer Is Empty
	while (!(UCSR0A & (1 << UDRE0)));

	// Write the Byte
	UDR0 = data;
}

void USART_WriteString(char *string)
{
	// Write characters out until we reach a NULL character
	while (*string != 0)
	{
		// Write the current character
		USART_Write((unsigned char)*string);
		// Next character
		string++;
	}
}

void USART_WriteCRLF(void)
{
	USART_Write(CR);
	USART_Write(LF);
}

void USART_WriteUnsignedChar(unsigned char number)
{
	unsigned char write_data = FALSE;
	// This number is at most 255 (3 digits)
	unsigned char ones = number % 10;
	number /= 10;
	unsigned char tens = number % 10;
	number /= 10;
	unsigned char hundreds = number % 10;
	
	if (hundreds > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + hundreds);
	if (tens > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + tens);
	USART_Write('0' + ones);
}

void USART_WriteUnsignedInt(unsigned int number)
{
	unsigned char write_data = FALSE;
	// This number is at most 65535 (5 digits)
	unsigned char digit1 = number % 10;
	number /= 10;
	unsigned char digit2 = number % 10;
	number /= 10;
	unsigned char digit3 = number % 10;
	number /= 10;
	unsigned char digit4 = number % 10;
	number /= 10;
	unsigned char digit5 = number % 10;

	if (digit5 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit5);
	if (digit4 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit4);
	if (digit3 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit3);
	if (digit2 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit2);
	USART_Write('0' + digit1);
}

void USART_WriteSignedInt(int number)
{
	// This number is at most +32768 and at least -32767 (5 digits + sign)s

	// Print the sign if applicable
	if (number < 0)
	{
		USART_Write('-');
		// Convert the number into an unsigned int
		number *= -1;
	}

	// Print out the positive number result
	USART_WriteUnsignedInt((unsigned int)number);
}

unsigned char USART_Receive(void)
{
	// Wait for data to be received
	while (!(UCSR0A & (1 << RXC0)));
	
	// Get and retrun received data from buffer
	return UDR0;
}

void USART_Flush(void)
{
	unsigned char dummy;
	
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}
