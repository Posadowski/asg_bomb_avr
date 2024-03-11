#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <avr/io.h>

// define your keyboar pins here
#define R1      PB4
#define R2      PB3
#define R3      PB2
#define R4      PB1
#define C1      PB0
#define C2      PD7
#define C3      PD6
#define C4      PD5

#define ROWS    4
#define COLS    4 

//initialize an instance of class NewKeypad

void keypad_init();

#endif //KEYPAD_H_