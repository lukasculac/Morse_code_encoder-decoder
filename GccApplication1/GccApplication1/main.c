#define F_CPU 7372800UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "lcd.h"
#include "uart_hal.h"

static char rx_buffer[RX_BUFFER_SIZE] = {0};
static char morse_buffer[RX_BUFFER_SIZE] = {0};
static bool mode_flag = false;
static uint16_t rxc_write_pos = 0;

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

//Buzzer controls
void sound_on(void){
	DDRB |= _BV(3);
}

void sound_off(void){
	DDRB &= ~_BV(3);
}

//Light controls
void lights_on(void){
	PORTA &= 0x00;
}

void lights_off(void){
	PORTA |= 0xff;
}

//Turn dots and dashes into light and sound signals
void buzz_light(const char* str){
	for(int i = 0; i < strlen(str); i++){

		if(str[i] == '.'){
			sound_on();
			lights_on();
			_delay_ms(100);
			
			sound_off();
			lights_off();
			_delay_ms(100);
		}
		else if(str[i] == '-'){
			sound_on();
			lights_on();
			_delay_ms(300);
			
			sound_off();
			lights_off();
			_delay_ms(100);
		}
		else if(str[i] == ' '){
			_delay_ms(200);
		}
		else if(str[i] == '/'){
			_delay_ms(500);
		}
	}
}

//Indexing morse code
int morse_to_index (const char* str)
{
	char sum = 0;
	uint8_t bit;

	for (bit = 1; bit; bit <<= 1) {
		switch (*str++) {
			case 0:
				return sum | bit;
			default:
				return 0;
			case '-':
				sum |= bit;
			case '.':
				break;
		}
	}
	return 0;
}

//Functions for coding and decoding morse code
const char* morse_to_char (const char* str)
{
	return MORSE_TO_CHAR[morse_to_index(str)];
}

const char* char_to_morse (char c)
{
	if (islower(c))
	c += ('A' - 'a');

	return CHAR_TO_MORSE[(int) c];
}


void code_string_input(const char* str){
	char coded_word[128] = {""};
	for (unsigned int i = 0; i < strlen(str) - 1; i++){
		if (str[i] == ' '){
			coded_word[strlen(coded_word)-1] = '\0';
			strcat(coded_word,"/");
			continue;
		}
		strcat(coded_word, char_to_morse(str[i]));
		strcat(coded_word," ");
	}
	lcd_clrscr();
	
	buzz_light(coded_word);
}

void decode_morse_code(const char* morse, uint8_t j){
	lcd_gotoxy(j,1);
	lcd_puts(morse_to_char(morse));
	for(int i = 0; i < 16; i++){
		lcd_gotoxy(i,0);
		lcd_puts(" ");
	}
	lcd_home();
}

void debounce() {
	_delay_ms(100);
	GIFR = _BV(INTF0);
}

//interrupts
ISR(INT0_vect){
	if(mode_flag == false){
		mode_flag = true;
		lights_on();
		lcd_clrscr();
	}
	else if (mode_flag == true){
		mode_flag = false;
		lights_off();
		lcd_clrscr();
	}
	debounce();
}

ISR(USARTRXC_vect){
	if(mode_flag == false){
		rx_buffer[rxc_write_pos] = UDR;
		
		if(rx_buffer[rxc_write_pos] == '0'){
			code_string_input(rx_buffer);
			rxc_write_pos = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));
			return;
		}
		rxc_write_pos++;
		if(rxc_write_pos >= RX_BUFFER_SIZE){
			rxc_write_pos = 0;
		}
	}
}

int main(void)
{
	int8_t j = 0;
	int8_t new_word = 1;
	uint16_t dot_counter = 0;
	uint16_t space_counter = 0;
	
	//LCD display 
	DDRD = _BV(4);
	DDRB |= _BV(0) | _BV(1);
	 
	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 1;

	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	
 	DDRA = 0xff;
    	PORTA = 0xff;
	
	//INT0 initialization
	MCUCR = _BV(ISC11) | _BV(ISC01);
	GICR = _BV(INT0);
	
	//uart initialization
	uart_init(9600, 0);
	
	sei();
	
        while (1) 
        {
		
		if(mode_flag == true){
			
			//dok nije pritisnut
			while(!(PINB & _BV(PB0))){
				space_counter++;
				_delay_ms(10);
				
				if(space_counter > 100 && morse_buffer[0] != '\0'){
					decode_morse_code(morse_buffer, j);
					memset(morse_buffer, 0, sizeof(morse_buffer));
					space_counter = 0;
					new_word = 0;
					j++;
				}
				else if(space_counter > 300 && new_word == 0){
					lcd_gotoxy(j,1);
					lcd_puts("_");
					space_counter = 0;
					new_word = 1;
					lcd_home();
					j++;
				}
				else if(space_counter > 1000){
					lcd_clrscr();
					j = 0;
					space_counter = 0;
					new_word = 1;
				}
			}
			
			//dok je pritisnut
			while(!(PINB & _BV(PB1))){
				space_counter = 0;
				sound_on();
				dot_counter++;
				_delay_ms(10);
			}
			sound_off();
			if(dot_counter < 24){
				strcat(morse_buffer,".");
				lcd_puts(".");
			}
			else if(dot_counter >= 24){
				strcat(morse_buffer,"-");
				lcd_puts("-");
			}
			
			dot_counter = 0;
		}
        }
}

