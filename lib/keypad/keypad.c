#include "keypad.h"

void keypad_init(){

 // Konfiguracja C1...C4 jako wyjścia i ustawienie na HIGH
    DDRB |= (1 << C1);  // C1
    PORTB |= (1 << C1);

    DDRD |= (1 << C4) | (1 << C3) | (1 << C2);  // C2, C3, C4
    PORTD |= (1 << C4) | (1 << C3) | (1 << C2);

    // Konfiguracja R1...R4 jako wejścia z pull-up
    DDRB &= ~((1 << R4) | (1 << R3) | (1 << R2) | (1 << R1));  // R1...R4
    PORTB |= (1 << R4) | (1 << R3) | (1 << R2) | (1 << R1);  // Włączenie pull-up
}