#include "memory.h"

#include <avr/eeprom.h>
#include <avr/interrupt.h>

void write_data_to_flash(mem_data *data) {
  cli();
  eeprom_write_block((const void *)data, (void *)0, sizeof(mem_data));
  sei();
}

void read_data_from_flash(mem_data *data) {
  cli();
  eeprom_read_block((void *)data, (const void *)0, sizeof(mem_data));
  sei();
}
