#include "keypad.h"

#include "../task_machinery/task_machinery.h"

#include <avr/io.h>
#include <util/delay.h>

extern task_queue *head;
char keypad_current_pressed_key = '\0';

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

void keypad_check_key_pressed(void *arg) {
   uint8_t keypad_state = KEYPAD_RELEASES;

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
                    if (!(PINB & (1 << R1_PIN))) {keypad_current_pressed_key = '1'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 1){
                    if (!(PINB & (1 << R1_PIN))) {keypad_current_pressed_key = '2'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 2){
                    if (!(PINB & (1 << R1_PIN))) {keypad_current_pressed_key = '3'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 3){
                    if (!(PINB & (1 << R1_PIN))) {keypad_current_pressed_key = 'A'; keypad_state = KEYPAD_PRESSED;}
                  } 
                    break;
                case 1:
                  if (col == 0){
                    if (!(PINB & (1 << R2_PIN))) {keypad_current_pressed_key = '4'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 1){
                    if (!(PINB & (1 << R2_PIN))) {keypad_current_pressed_key = '5'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 2){
                    if (!(PINB & (1 << R2_PIN))) {keypad_current_pressed_key = '6'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 3){
                    if (!(PINB & (1 << R2_PIN))) {keypad_current_pressed_key = 'B'; keypad_state = KEYPAD_PRESSED;}
                  }  
                    
                    break;
                case 2:
                  if (col == 0){
                    if (!(PINB & (1 << R3_PIN))) {keypad_current_pressed_key = '7'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 1){
                    if (!(PINB & (1 << R3_PIN))) {keypad_current_pressed_key = '8'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 2){
                    if (!(PINB & (1 << R3_PIN))) {keypad_current_pressed_key = '9'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 3){
                    if (!(PINB & (1 << R3_PIN))) {keypad_current_pressed_key = 'C'; keypad_state = KEYPAD_PRESSED;}
                  } 
                    break;
                case 3:
                  if (col == 0){
                    if (!(PINB & (1 << R4_PIN))) {keypad_current_pressed_key = '*'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 1){
                    if (!(PINB & (1 << R4_PIN))) {keypad_current_pressed_key = '0'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 2){
                    if (!(PINB & (1 << R4_PIN))) {keypad_current_pressed_key = '#'; keypad_state = KEYPAD_PRESSED;}
                  } else if (col == 3){
                    if (!(PINB & (1 << R4_PIN))){ 
                      keypad_current_pressed_key = 'D'; 
                      keypad_state = KEYPAD_PRESSED;
                    }
                  }    
                    break;
            }
        }
    }
    if(keypad_state != KEYPAD_PRESSED){
      keypad_current_pressed_key = '\0'; //If there is no button pressed
    }
    taskMachinery_engque(&head,_KEYPAD_CHECK_TIME,keypad_check_key_pressed,NULL);
}


char keypad_get_last_pressed_key(){
   return keypad_current_pressed_key;
}