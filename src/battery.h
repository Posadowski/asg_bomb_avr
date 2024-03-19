#ifndef BATTERY_H
#define BATTERY_H

#include "../lib/lcd/lcd.h"

void check_battery(LiquidCrystalDevice_t* lcd);
void ADC_init() ;

#endif //BATTERY_H