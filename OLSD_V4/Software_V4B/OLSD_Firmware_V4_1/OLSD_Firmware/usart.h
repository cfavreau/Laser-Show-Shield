// *********************************************************************************
// usart.h
// *********************************************************************************

#ifndef __USART_H__
#define __USART_H__

// CTS Control (Clear To Send) PORTD Bit 7
#define CTS_ON	portd.bit7 = 0
#define CTS_OFF	portd.bit7 = 1

#define READ(data) while (!(UCSR0A & (1 << RXC0))); data = UDR0;

void USART_Init(void);
void USART_Write(unsigned char data);
void USART_WriteString(char *string);
void USART_WriteCRLF(void);
void USART_WriteUnsignedChar(unsigned char number);
void USART_WriteUnsignedInt(unsigned int number);
void USART_WriteSignedInt(int number);
unsigned char USART_Receive(void);
void USART_Flush(void);

#endif // __USART_H__
