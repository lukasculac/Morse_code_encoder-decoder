/*
Copyright (c) 2021 Devilbinder (Binder Tronics) https://bindertronics.com/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */ 

#include "uart_hal.h"
#include "lcd.h"
volatile static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static uint16_t rx_count = 0;	
volatile static uint8_t uart_tx_busy = 1;

/*ISR(USARTRXC_vect){
	PORTA &= 0xfe;
	volatile static uint16_t rxc_write_pos = 0;
	
	rx_buffer[rxc_write_pos] = UDR;
	rx_count++;
	rxc_write_pos++;
	if(rxc_write_pos >= RX_BUFFER_SIZE){
		rxc_write_pos = 0;
	}
	
}*/


ISR(USARTTXC_vect){
	PORTA &= 0xfd;
	uart_tx_busy = 1;
}


void uart_init(uint32_t baud,uint8_t high_speed){
	
	uint8_t speed = 16;
	
	if(high_speed != 0){
		speed = 8;
		UCSRA |= 1 << U2X;
	}
	
	baud = (F_CPU/(speed*baud)) - 1;
	
	UBRRH = (baud & 0x0F00) >> 8;
	UBRRL = (baud & 0x00FF);
	
	UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << TXCIE) | (1 << RXCIE);
	UCSRC &= ~_BV(URSEL);
}


void uart_send_byte(uint8_t c){
	while(uart_tx_busy == 0);
	uart_tx_busy = 0;
	UDR = c;
}

void uart_send_array(uint8_t *c,uint16_t len){
	for(uint16_t i = 0; i < len;i++){
		uart_send_byte(c[i]);
	}
}

void uart_send_string(uint8_t *c){
	uint16_t i = 0;
	do{
		uart_send_byte(c[i]);
		i++;
		
	}while(c[i] != '\0');
	uart_send_byte(c[i]);
}

uint16_t uart_read_count(void){
	return rx_count;
}

uint8_t uart_read(void){
	static uint16_t rx_read_pos = 0;
	uint8_t data = 0;
	
	data = rx_buffer[rx_read_pos];
	rx_read_pos++;
	rx_count--;
	if(rx_read_pos >= RX_BUFFER_SIZE){
		rx_read_pos = 0;
	}
	return data;
}