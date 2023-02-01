#define F_CPU7372800UL

#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>

ISR(USARTRXC_vect) {
	PORTA |= 0x01;
}


unsigned char USART_getc( void )
{
	// Wait for data to be received
	while ( !(UCSRA & _BV(RXC)) );
	// Get and return received data from buffer
	PORTA |= 0x01;
	return 0;
}

void UART_Init(uint16_t v_baudRate_u16)
{
	
	UCSRA= 0x00;                 // Clear the UASRT status register
	UCSRB= (1<<RXEN) | (1<<RXCIE) | (1 << TXEN);     // Enable Receiver and Transmitter
	UCSRC= (1<<UCSZ1) | (1<<UCSZ0);   // Async-mode
	
	UBRRL = v_baudRate_u16;
	UBRRH = (v_baudRate_u16 >> 8);
	PORTA |= 0x04;
}


int main(void)
{
	
	DDRA = 0xff;
	PORTA = 0x00;
	UART_Init(3);
	sei();
    while (1) 
    {
		USART_getc();
    }
}

