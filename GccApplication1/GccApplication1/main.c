#define F_CPU 7372800UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lcd.h"
#include "uart_hal.h"


volatile static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static uint16_t rx_count = 0;


static char word_to_code[32];
static char morse_to_decode[128];
static char decoded_morse[32] = {""};
static char coded_word[128] = {""};

//Storage for data
static const char* CHAR_TO_MORSE[128] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, "-.-.--", ".-..-.", NULL, NULL, NULL, NULL, ".----.",
	"-.--.", "-.--.-", NULL, NULL, "--..--", "-....-", ".-.-.-", "-..-.",
	"-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
	"---..", "----.", "---...", NULL, NULL, "-...-", NULL, "..--..",
	".--.-.", ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
	"....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
	".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--",
	"-..-", "-.--", "--..", NULL, NULL, NULL, NULL, "..--.-",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static const char* MORSE_TO_CHAR[128] = {
	NULL, NULL, "E", "T", "I", "N", "A", "M",
	"S", "D", "R", "G", "U", "K", "W", "O",
	"H", "B", "L", "Z", "F", "C", "P", NULL,
	"V", "X", NULL, "Q", NULL, "Y", "J", NULL,
	"5", "6", NULL, "7", NULL, NULL, NULL, "8",
	NULL, "/", NULL, NULL, NULL, "(", NULL, "9",
	"4", "=", NULL, NULL, NULL, NULL, NULL, NULL,
	"3", NULL, NULL, NULL, "2", NULL, "1", "0",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, ":",
	NULL, NULL, NULL, NULL, "?", NULL, NULL, NULL,
	NULL, NULL, "\"", NULL, NULL, NULL, "@", NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, "'", NULL,
	NULL, "-", NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, ".", NULL, "_", ")", NULL, NULL,
	NULL, NULL, NULL, ",", NULL, "!", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void debounce() {
	_delay_ms(100);
	GIFR = _BV(INTF0) | _BV(INTF1);
}

ISR(INT0_vect){
	DDRB |= _BV(3);
	
	while(!(PIND & _BV(PD2))){
		PORTA &= ~_BV(1);
	}
	PORTA |= _BV(1);
	
	debounce();
}

ISR(INT1_vect){
	
	debounce();
}

ISR(USARTRXC_vect){
	PORTA &= 0xfe;
	volatile static uint16_t rxc_write_pos = 0;
	
	rx_buffer[rxc_write_pos] = UDR;
	if(rx_buffer[rxc_write_pos] == 'A'){
		DDRB |= _BV(0);
		_delay_ms(300);
		DDRB &= ~_BV(0);
		_delay_ms(100);
		DDRB |= _BV(0);
		_delay_ms(100);
		DDRB &= ~_BV(0);
		_delay_ms(100);
		DDRB |= _BV(0);
		_delay_ms(300);
		DDRB &= ~_BV(0);
	}
	lcd_clrscr();
	lcd_data(rx_buffer[rxc_write_pos]);
	
	rx_count++;
	rxc_write_pos++;
	if(rxc_write_pos >= RX_BUFFER_SIZE){
		rxc_write_pos = 0;
	}
	
}

//Indexing morse code
int morse_to_index (const char* str)
{
	unsigned char sum = 0, bit;
	//printf("%s", str);
	for (bit = 1; bit; bit <<= 1) {
		switch (*str++) {
			case 0:
			return sum | bit;
			default:
			return 0;
			case '-':
			sum |= bit;
			/* FALLTHROUGH */
			case '.':
			break;
		}
	}
	
	return 0;
}

//Functions for coding and decoding morse code
const char* char_to_morse (char c)
{
	if (islower(c))
	c += ('A' - 'a');

	return CHAR_TO_MORSE[(int) c];
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
	
	MCUCR = _BV(ISC11) | _BV(ISC01);
	GICR = _BV(INT0) | _BV(INT1);
	
	//uart initialization
	uart_init(9600, 0);

	sei();
	
    while (1) 
    {
		DDRB &= ~_BV(3);
	}
}

