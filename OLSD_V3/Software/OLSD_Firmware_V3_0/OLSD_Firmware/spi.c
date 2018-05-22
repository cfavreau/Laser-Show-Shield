// *********************************************************************************
// spi.c
// *********************************************************************************

#include <avr/io.h>
#include "spi.h"

// SPI Data Register Definitions
#define DDR_SPI   DDRB
#define DD_MOSI   DDB3
#define DD_MISO   DDB4
#define DD_SCK    DDB5

void SPI_Init(void)
{
   DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK);
   // Enable SPI Master and Set Clock rate to CLK / 2 (I.E. 16MHz)
   // and MSB first and Clock Polarity is Rising on the Leading Edge (Mode 0).
   SPCR |= (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (0 << SPR0);
   SPSR |= (1 << SPI2X); //We need the fast 2X mode.
}

void SPI_WriteBYTE(unsigned char data)
{
   // *** Do not toggle the slave select lines.  Leave this up to who ever is calling us.

   SPDR = data; // Start the transmission by writing the data into the SPI Out Data Register.
   while (!(SPSR & (1 << SPIF))); // Wait for the previous transmission to complete.
}

void SPI_WriteWORD(unsigned int data)
{
   //USART_WriteString("SPI Write WORD Hi(");
   // Write the High Byte
   SPI_WriteBYTE((unsigned char)(data >> 8));
   //USART_WriteUnsignedChar((unsigned char)(data >> 8));
   //USART_WriteString(") Lo(");
   // Write the Low Byte
   SPI_WriteBYTE((unsigned char)(data & 0x00FF));
   //USART_WriteUnsignedChar((unsigned char)(data & 0x00FF));
   //USART_WriteString(")\r\n");
}
