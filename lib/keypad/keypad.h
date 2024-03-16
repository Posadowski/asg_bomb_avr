#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <avr/io.h>

// define your keyboar pins here
//rows
#define R1_PIN      PB4
#define R1_PORT     PORTB
#define R1_DDR      DDRB

#define R2_PIN      PB3
#define R2_PORT     PORTB
#define R2_DDR      DDRB

#define R3_PIN      PB2
#define R3_PORT     PORTB
#define R3_DDR      DDRB

#define R4_PIN      PB1
#define R4_PORT     PORTB
#define R4_DDR      DDRB

//colums
#define C1_PIN      PB0
#define C1_PORT     PORTB
#define C1_DDR      DDRB

#define C2_PIN      PD7
#define C2_PORT     PORTD
#define C2_DDR      DDRD

#define C3_PIN      PD6
#define C3_PORT     PORTD
#define C3_DDR      DDRD

#define C4_PIN      PD5
#define C4_PORT     PORTD
#define C4_DDR      DDRD

#define ROWS    4
#define COLS    4 

#define _KEY_PRESSED_POS (1)
#define _KEY_PRESSED (1UL << _KEY_PRESSED_POS)

#define _JUST_PRESSED_POS (2)
#define _JUST_PRESSED (1UL << _JUST_PRESSED_POS)

#define _JUST_RELEASED_POS (3)
#define _JUST_RELEASED (1UL << _JUST_RELEASED_POS)

#define KEY_JUST_RELEASED (0) ///< key has been released
#define KEY_JUST_PRESSED (1)  ///< key has been pressed

#define _KEYPAD_SETTLING_DELAY 20

void keypad_init();
char keyboard_check_key_pressed();

#endif //KEYPAD_H_