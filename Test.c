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




static char word_to_decode[32];
static char dots_and_dashes[128];

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

const char* char_to_morse (char c)
{
        if (islower(c))
                c += ('A' - 'a');

        return CHAR_TO_MORSE[(int) c];
}

int main() {  

    //getting sentence and storing in char array 
    printf("Enter a string: "); 
    fflush(stdin);
    fgets(word_to_decode, sizeof(word_to_decode), stdin);
     
    char decoded_word[128] = {""};
    
    
    for (int i = 0; i < strlen(word_to_decode) - 1; i++){
        if (word_to_decode[i] == ' '){
            strcat(decoded_word,"|");
            continue;
        }
        strcat(decoded_word, char_to_morse(word_to_decode[i]));
        strcat(decoded_word," ");
    }
    printf("%s", decoded_word);
    
    return 0;
}