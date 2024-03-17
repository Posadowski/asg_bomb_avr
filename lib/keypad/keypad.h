#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <avr/io.h>

// define your keyboar pins here
// rows
#define R1_PIN PB4
#define R2_PIN PB3
#define R3_PIN PB2
#define R4_PIN PB1
// colums
#define C1_PIN PB0
#define C2_PIN PD7
#define C3_PIN PD6
#define C4_PIN PD5

#define ROWS 4
#define COLS 4

#define KEYPAD_PRESSED 1
#define KEYPAD_RELEASES 0

#define _KEYPAD_CHECK_TIME 5

#define ENTER_KEY 'D'
#define PRESET_1_KEY 'A'
#define PRESET_2_KEY 'B'
#define PRESET_3_KEY 'C'

void keypad_init();
void keypad_check_key_pressed(void *arg);
char keypad_get_last_pressed_key();

#endif // KEYPAD_H_