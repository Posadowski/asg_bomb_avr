#include "battery.h"

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "../lib/keypad/keypad.h"
#include "../lib/task_machinery/task_machinery.h"
#include "../lib/usart/usart.h"
// measure 3,2V -> adc 820
#define RANGE_MIN 704   // 2,75V cut-off li-on voltage
#define RANGE_MAX 1012  // 3,95V max voltage from charger

#define TRUE 1
#define FALSE 0

#define CPU_GAUGE_SAMLES 20

extern task_queue *head;

uint16_t cpu_gauge_buffer[CPU_GAUGE_SAMLES] = {};
uint16_t cpu_gauge = 0;  // value for the CPU gauge

uint8_t cpu_gauge_index = 0;
uint8_t cpu_gauge_ready = FALSE;

 uint8_t battery_checked = FALSE;

uint16_t constrain(uint16_t value, uint16_t min, uint16_t max) {
  if (value < min)
    return min;
  else if (value > max)
    return max;
  else
    return value;
}

uint16_t converterPerPercent(uint16_t adcValue) {
  // Limiting values ​​to a range
  adcValue = constrain(adcValue, RANGE_MIN, RANGE_MAX);
  // Convert values ​​to percentages
  uint16_t percent = map(adcValue, RANGE_MIN, RANGE_MAX, 0, 100);
  return percent;
}

double round(double x) { return floor(x + 0.5); }

void ADC_init() {
  // Setting the reference voltage to AVCC with an external capacitor at the
  // AREF pin input
  ADMUX |= (1 << REFS0);
  // Setting the prescaler to 128 128 (fadc = fcpu/128)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  // Enable ADC
  ADCSRA |= (1 << ADEN);
}

uint16_t ADC_read(uint8_t channel) {
  // ADC channel setting
  ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
  // ADC conversion begins
  ADCSRA |= (1 << ADSC);
  // Waiting for the conversion to complete
  while (ADCSRA & (1 << ADSC));
  // Returning the conversion result (16-bit value)
  return ADC;
}

void measure_battery_level(void *arg){
      int cpu_gauge_current = 0;
      cpu_gauge_buffer[cpu_gauge_index] = converterPerPercent(ADC_read(0));
      static uint8_t buffer_filled = FALSE;
      cpu_gauge_index = (cpu_gauge_index + 1) % CPU_GAUGE_SAMLES;
      if(cpu_gauge_index == CPU_GAUGE_SAMLES-1){
        buffer_filled = TRUE;
      }
      uint8_t i;
      for (i = 0; i < CPU_GAUGE_SAMLES; i++) {
        cpu_gauge_current += cpu_gauge_buffer[i];
        if (cpu_gauge_buffer[i] != 0 || buffer_filled == TRUE) {
          cpu_gauge_ready = TRUE;
        } else {
          cpu_gauge_ready = FALSE;
        }
      }
     
      cpu_gauge_current = cpu_gauge_current / CPU_GAUGE_SAMLES;
      if (cpu_gauge != cpu_gauge_current) {
        cpu_gauge = cpu_gauge_current;
      }
      if(battery_checked == FALSE){
        taskMachinery_engque(&head, 100, measure_battery_level, NULL);
      }
}

void check_battery(LiquidCrystalDevice_t* lcd) {
  battery_checked = FALSE;
  taskMachinery_engque(&head, 100, measure_battery_level, NULL);
  lq_clear(lcd);

  char old_key_pressed = '\0';
  char buffer[LCD_LINE_LENGTH];
  int old_cpu_gauge = 0;

  while (battery_checked == FALSE) {
    // gauge drawing
    lq_setCursor(lcd, 0, 0);  // move cursor to top left
    if (cpu_gauge_ready == TRUE) {
      sprintf(buffer, "Battery :%3d%% ",
              cpu_gauge);  // set a string as CPU: XX%, with the number always
                           // taking at least 3 character
    } else {
      sprintf(buffer, "Measure...");
    }
    if(old_cpu_gauge != cpu_gauge){
      
      lq_print(lcd, buffer);              // print the string on the display
      //lq_writeDevice4Bits(lcd, (char)(0));  // print warning character
      lq_print(lcd, "  ");
    }
 

    // if(old_cpu_gauge != cpu_gauge){
    //   lq_setCursor(lcd, 1, 0);      // move the cursor to the next line
    //   lq_print(lcd, gauge_string);  // display the gauge
    // }

    old_cpu_gauge = cpu_gauge;
    char key_pressed = keypad_get_last_pressed_key();
    if (old_key_pressed != key_pressed) {
      if (key_pressed == (char)ENTER_KEY) {
        printf("battery checked!!!!\n");
        battery_checked = TRUE;
        lq_clear(lcd);
        return;
      }

      old_key_pressed = key_pressed;
    }
    _delay_ms(10);
  }
}