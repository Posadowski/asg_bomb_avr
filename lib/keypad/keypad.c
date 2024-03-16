#include "keypad.h"

#include <avr/io.h>
#include <util/delay.h>

void keypad_init(){

 // Configure C1...C4 as outputs and set to HIGH
    DDRB |= (1 << C1_PIN);  // C1
    PORTB |= (1 << C1_PIN);

    DDRD |= (1 << C4_PIN) | (1 << C3_PIN) | (1 << C2_PIN);  // C2, C3, C4
    PORTD |= (1 << C4_PIN) | (1 << C3_PIN) | (1 << C2_PIN);

    // Configuration of R1...R4 as pull-up inputs
    DDRB &= ~((1 << R4_PIN) | (1 << R3_PIN) | (1 << R2_PIN) | (1 << R1_PIN));  // R1...R4
    PORTB |= (1 << R4_PIN) | (1 << R3_PIN) | (1 << R2_PIN) | (1 << R1_PIN);  // Enable pull-up

}

char keyboard_check_key_pressed(void *arg) {
    for (int col = 0; col < 4; col++) {
        switch (col) {
            case 0:
                C1_PORT &= ~(1 << C1_PIN);
                C2_PORT |= (1 << C2_PIN);
                C3_PORT |= (1 << C3_PIN);
                C4_PORT |= (1 << C4_PIN);
                break;
            case 1:
                C1_PORT |= (1 << C1_PIN);
                C2_PORT &= ~(1 << C2_PIN);
                C3_PORT |= (1 << C3_PIN);
                C4_PORT |= (1 << C4_PIN);
                break;
            case 2:
                C1_PORT |= (1 << C1_PIN);
                C2_PORT |= (1 << C2_PIN);
                C3_PORT &= ~(1 << C3_PIN);
                C4_PORT |= (1 << C4_PIN);
                break;
            case 3:
                C1_PORT |= (1 << C1_PIN);
                C2_PORT |= (1 << C2_PIN);
                C3_PORT |= (1 << C3_PIN);
                C4_PORT &= ~(1 << C4_PIN);
                break;
        }
        
        
        for (int row = 0; row < 4; row++) {
            switch (row) {
                case 0:
                  if(col == 0){
                    if (!(PINB & (1 << R1_PIN))) return '1';
                  } else if (col == 1){
                    if (!(PINB & (1 << R1_PIN))) return '2';
                  } else if (col == 2){
                    if (!(PINB & (1 << R1_PIN))) return '3';
                  } else if (col == 3){
                    if (!(PINB & (1 << R1_PIN))) return 'A';
                  } 
                    break;
                case 1:
                  if (col == 0){
                    if (!(PINB & (1 << R2_PIN))) return '4';
                  } else if (col == 1){
                    if (!(PINB & (1 << R2_PIN))) return '5';
                  } else if (col == 2){
                    if (!(PINB & (1 << R2_PIN))) return '6';
                  } else if (col == 3){
                    if (!(PINB & (1 << R2_PIN))) return 'B';
                  }  
                    
                    break;
                case 2:
                  if (col == 0){
                    if (!(PINB & (1 << R3_PIN))) return '7';
                  } else if (col == 1){
                    if (!(PINB & (1 << R3_PIN))) return '8';
                  } else if (col == 2){
                    if (!(PINB & (1 << R3_PIN))) return '9';
                  } else if (col == 3){
                    if (!(PINB & (1 << R3_PIN))) return 'C';
                  } 
                    break;
                case 3:
                  if (col == 0){
                    if (!(PINB & (1 << R4_PIN))) return '*';
                  } else if (col == 1){
                    if (!(PINB & (1 << R4_PIN))) return '0';
                  } else if (col == 2){
                    if (!(PINB & (1 << R4_PIN))) return '#';
                  } else if (col == 3){
                    if (!(PINB & (1 << R4_PIN))) return 'D';
                  }    
                    break;
            }
        }
    }
    return '\0'; //If there is no button pressed
}