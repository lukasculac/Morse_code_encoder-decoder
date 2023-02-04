#define F_CPU 7372800UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include "lcd.h"
#include "uart_hal.h"


volatile static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static uint16_t rx_count = 0;

ISR(USARTRXC_vect){
	PORTA &= 0xfe;
	volatile static uint16_t rxc_write_pos = 0;
	
	rx_buffer[rxc_write_pos] = UDR;
	if(rx_buffer[rxc_write_pos] == 'A'){
		DDRD |= _BV(3);
		_delay_ms(100);
		DDRD &= ~_BV(3);
		_delay_ms(70);
		DDRD |= _BV(3);
		_delay_ms(300);
		DDRD &= ~_BV(3);
	}
	
	lcd_clrscr();
	lcd_data(rx_buffer[rxc_write_pos]);
	
	rx_count++;
	rxc_write_pos++;
	if(rxc_write_pos >= RX_BUFFER_SIZE){
		rxc_write_pos = 0;
	}
	
}

int main(void)
{
	 //LCD display 
	 DDRD = _BV(4);
	 
	 TCCR1A = _BV(COM1B1) | _BV(WGM10);
	 TCCR1B = _BV(WGM12) | _BV(CS11);
	 OCR1B = 1;

	 lcd_init(LCD_DISP_ON);
	 lcd_clrscr();
	 
 	DDRA = 0xff;
    PORTA = 0xff;
	
	//uart initialization
	uart_init(9600, 0);

	sei();
	
    while (1) 
    {}
}

