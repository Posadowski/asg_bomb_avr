#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include "../lib/lcd/lcd.h"
#include "../lib/memory/memory.h"

#define SETTINGS_ENTER_KEY '#'
#define SETTINGS_UP_KEY 'A'
#define SETTINGS_DOWN_KEY 'B'
#define SETTINGS_CLEAR_KEY 'C'

void settings_menu(LiquidCrystalDevice_t* lcd);

char* settings_password_set(char* password_new_value);
char* settings_password_get();

uint16_t settings_preset1_set(uint16_t preset1_new_value);
uint16_t settings_preset1_get();

uint16_t settings_preset2_set(uint16_t preset2_new_value);
uint16_t settings_preset2_get();

uint16_t settings_preset3_set(uint16_t preset3_new_value);
uint16_t settings_preset3_get();

#endif  // SETTINGS_MENU