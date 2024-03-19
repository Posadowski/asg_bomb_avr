#ifndef BOMB_MACHINERY_H
#define BOMB_MACHINERY_H

#include <avr/io.h>

#include "../lib/lcd/lcd.h"

#define BUZZER_PIN PD3

void activateBomb(uint16_t timeToExplode, const char* PassToDefused,
                  LiquidCrystalDevice_t* lcd);

#endif  // BOMB_MACHINERY_H