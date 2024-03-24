#ifndef MEMORY_H_
#define MEMORY_H_

#include <avr/io.h>

#define MAX_PASSWORD_LENGTH 15

typedef struct mem_data {
  int8_t mem_init;
  uint16_t mem_preset1;
  uint16_t mem_preset2;
  uint16_t mem_preset3;
  uint8_t mem_volume;
  char password[MAX_PASSWORD_LENGTH];  // +1 dla znaku końca łańcucha
} mem_data;

#define EEPROM_ADDR_INIT 0
#define EEPROM_ADDR_PRESET1 2
#define EEPROM_ADDR_PRESET2 4
#define EEPROM_ADDR_PRESET3 6
#define EEPROM_ADDR_PASSWORD 10
#define EEPROM_ADDR_VOLUME 40

void write_data_to_flash(mem_data *data);
void read_data_from_flash(mem_data *data);
#endif  // MEMORY_H_