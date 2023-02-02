#define F_CPU7372800UL

/*
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static char word_to_code[32];
static char morse_to_decode[128];


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

const char* morse_to_char (const char* str)
{
        
        return MORSE_TO_CHAR[morse_to_index(str)];
}




int main() {  

    
    //coding a word to morse part
    printf("Enter a string: "); 
    fflush(stdin);
    fgets(word_to_code, sizeof(word_to_code), stdin);
     
    char coded_word[128] = {""};
    
    for (int i = 0; i < strlen(word_to_code) - 1; i++){
        if (word_to_code[i] == ' '){
            strcat(coded_word,"|");
            continue;
        }
        strcat(coded_word, char_to_morse(word_to_code[i]));
        strcat(coded_word," ");
    }
    printf("Your string in morse code is:\n%s\n", coded_word);
    
    
    //decoding morse code part
    char decoded_morse[128] = {""};
    printf("Enter a morse code: "); 
    fflush(stdin);
    fgets(morse_to_decode, sizeof(morse_to_decode), stdin);
    
    char temp[8] = {""};
    
    for (unsigned int i = 0; i < strlen(morse_to_decode) - 1; i++){
        
        if(morse_to_decode[i] == ' '){
            
            strcat(decoded_morse, morse_to_char(temp));
            temp[0] = 0;
            
            continue;         
            } 

        else if(morse_to_decode[i] == '/'){
            strcat(decoded_morse, morse_to_char(temp));
            strcat(decoded_morse, " ");
            temp[0] = 0;
            continue;
        }

        strncat(temp, &morse_to_decode[i], 1);
    }
    strcat(decoded_morse, morse_to_char(temp));
    printf("Your morse code says:\n%s", decoded_morse);
    
    return 0;
}