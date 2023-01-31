#define F_CPU7372800UL

#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>

ISR(USART_RXC_vect) {
	
	PORTA = _BV(0);
}


void UART_Init(uint16_t v_baudRate_u16)
{
	UCSRA= _BV(U2X);                 // Clear the UASRT status register
	UCSRB= (1<<RXEN) | (1<<RXCIE);     // Enable Receiver and Transmitter
	UCSRC= (1<<UCSZ1) | (1<<UCSZ0);   // Async-mode
	
	UBRRL = v_baudRate_u16;
	UBRRH = v_baudRate_u16;
}


int main(void)
{
	
	DDRA = 0xff;
	PORTA = 0xff;
	UART_Init(95);
	
    while (1) 
    {
    }
}

