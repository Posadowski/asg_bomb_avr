
/* ASG bomb  */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "../lib/keypad/keypad.h"
#include "../lib/lcd/lcd.h"
#include "../lib/memory/memory.h"
#include "../lib/task_machinery/task_machinery.h"
#include "../lib/usart/usart.h"
#include "battery.h"
#include "bomb_machinery.h"
#include "settings_menu.h"
#include "tone.h"

//#define DEVELOP_VERSION

#ifdef DEVELOP_VERSION
#define SOFTWARE_VERSION "0.5 DEV"  // 0.3 last arduino version
#else
#define SOFTWARE_VERSION "0.5"  // 0.3 last arduino version
#endif

#ifdef F_CPU
#undef F_CPU
#endif



#define F_CPU 16000000UL
#define TIMER1_PRESCALER 1024
#define TIMER1_COMPARE_MATCH (F_CPU / TIMER1_PRESCALER / 1000)  // 1 mili second

#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

task_queue *head = NULL;
mem_data memory = {};

char enteredCommandTable[LCD_LINE_LENGTH];

void queue_test(void *arg) {
  taskMachinery_engque(&head, 5000, queue_test, NULL);
}

extern unsigned int __heap_start;
extern void *__brkval;

// Function to calculate free heap size
int freeHeap() {
  int free_memory;
  if ((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}

#ifdef DEVELOP_VERSION
void print_task_queue(void *arg) {
  printf("task queue \n");
  task_queue *current = (task_queue *)arg;
  uint8_t i = 0;
  while (current != NULL) {
    printf("%u: Time to execute: %u\n", i, current->time_to_execute);
    // You can print other members of the task_queue struct here if needed
    current = current->next;
    i++;
  }
  printf("freeHeap: %d\n", freeHeap());
  taskMachinery_engque(&head, 5000, print_task_queue, head);
}
#endif

FILE USART_Transmit_stream =
    FDEV_SETUP_STREAM(USART_Transmit_printf, NULL, _FDEV_SETUP_WRITE);

ISR(TIMER1_COMPA_vect) {
  task_queue *current = head;
  while (current != NULL) {
    current->time_to_execute--;
    if (current->time_to_execute == 0) {
      if (current->callback != NULL) {
        current->callback(current->data);
      }
      // Remove an item from the queue
      if (current->prev != NULL) {
        current->prev->next = current->next;
      } else {
        head = current->next;
      }
      if (current->next != NULL) {
        current->next->prev = current->prev;
      }
      task_queue *temp = current;
      current = current->next;
      free(temp);
    } else {
      current = current->next;
    }
  }
}

void init_timer() {
  TCCR1B |= (1 << WGM12);               // CTC Mode
  TIMSK1 |= (1 << OCIE1A);              // Allow interruption for comparison
  OCR1A = TIMER1_COMPARE_MATCH;         // Set the comparison time
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Prescaler 1024
  sei();                                // turn on intterupt
}

void playSound(int frequency) {
  uint8_t prescalarbits = 0b001;
  uint32_t ocr = 0;
  
  /*
  CS22  CS21 CS20 | prescaler select
  0      0     0  |  No clock source
  0      0     1  |  Clk(T2S)/No prescaler
  0      1     0  |  Clk(T2S)/8
  0      1     1  |  Clk(T2S)/32
  1      0     0  |  Clk(T2S)/64
  1      0     1  |  Clk(T2S)/128
  1      1     0  |  Clk(T2S)/256
  1      1     1  |  Clk(T2S)/1024
  */

  ocr = F_CPU / frequency / 2 - 1;
  if (ocr > 255) {
    ocr = F_CPU / frequency / 2 / 8 - 1;
    prescalarbits = 0b010;
    if (ocr > 255) {
      ocr = F_CPU / frequency / 2 / 32 - 1;
      prescalarbits = 0b011;
      if (ocr > 255) {
        ocr = F_CPU / frequency / 2 / 128 - 1;
        prescalarbits = 0b101;

        if (ocr > 255) {
          ocr = F_CPU / frequency / 2 / 256 - 1;
          prescalarbits = 0b110;
          if (ocr > 255) {
            // can't do any better than /1024
            ocr = F_CPU / frequency / 2 / 1024 - 1;
            prescalarbits = 0b111;
          }
        }
      }
    }
  }

  TCCR2B = (TCCR2B & 0b11111000) | prescalarbits;

  OCR2A = ocr;
  OCR2B = 110;
}

void papaj_event() {
  cli();
  setupPWM();
  OCR2B = 0;
    playSound(NOTE_C4);

    _delay_ms(1500);

    playSound(NOTE_A4);
    _delay_ms(1000);
    OCR2B = 0;
    _delay_ms(50);
    playSound(NOTE_A4);
    _delay_ms(1000);
    OCR2B = 0;
    _delay_ms(50);
    playSound(NOTE_A4);
    _delay_ms(700);
    playSound(NOTE_B4);
    _delay_ms(700);
    playSound(NOTE_C4);
    _delay_ms(700);
    playSound(NOTE_B4);
    _delay_ms(700);
    playSound(NOTE_A4);
    _delay_ms(1000);
    ///
    playSound(NOTE_G4);
    _delay_ms(1000);
    OCR2B = 0;
    _delay_ms(500);
    playSound(NOTE_F4);
    _delay_ms(1000);
    playSound(NOTE_E4);
    _delay_ms(700);
    playSound(NOTE_F4);
    _delay_ms(1000);
    OCR2B = 0;
    playSound(NOTE_F4);
    _delay_ms(1000);
    OCR2B = 0;
    playSound(NOTE_F4);
    _delay_ms(700);
    playSound(NOTE_G4);
    _delay_ms(700);
    playSound(NOTE_A4);
    _delay_ms(700);
    playSound(NOTE_G4);
    _delay_ms(700);
    playSound(NOTE_F4);
    _delay_ms(700);
    playSound(NOTE_E4);
    _delay_ms(500);
    OCR2B = 0;
    playSound(NOTE_E4);
    _delay_ms(500);

    disablePWM();
  
  PORTD &= ~(1 << BUZZER_PIN);  // pin buzzer LOW
  sei();
}

int main(void) {
  USART_Init(MYUBRR);
  stdout = &USART_Transmit_stream;
  //_delay_ms(2000);
  DDRB = 0xff;

  LiquidCrystalDevice_t device = lq_init(
      0x27, LCD_LINE_LENGTH, 2, LCD_5x8DOTS);  // intialize 4-lines display
  lq_turnOnBacklight(&device);  // simply turning on the backlight

  lq_clear(&device);
  lq_setCursor(&device, 0, 0);
  lq_print(&device, "ASG Bomb!");
  lq_setCursor(&device, 1, 0);
  char software_version[LCD_LINE_LENGTH] = "ver.";
  strcat(software_version, SOFTWARE_VERSION);
  lq_print(&device, software_version);
  _delay_ms(1000);

  ADC_init();

  keypad_init();  // define pins in lib/keypad/keypad.h
  #ifdef DEVELOP_VERSION
  taskMachinery_engque(&head, 5000, print_task_queue, NULL);
  #endif
  taskMachinery_engque(&head, _KEYPAD_CHECK_TIME, keypad_check_key_pressed,
                       NULL);
  init_timer();

  read_data_from_flash(&memory);
 
  if (memory.mem_init == 0) {
    // MEMORY INIT
    memory.mem_preset1 = 10;
    memory.mem_preset2 = 15;
    memory.mem_preset3 = 20;
    memory.mem_volume = 100;
    strcpy(memory.password, "1234");

    memory.mem_init = 1;
    write_data_to_flash(&memory);
  } else {
    printf("memory.mem_volume: %d\n",memory.mem_volume);
    printf("memory.password: %s\n",memory.password);

  }
  lq_clear(&device);
  lq_setCursor(&device, 0, 0);

   DDRD |= (1 << BUZZER_PIN); // set pin buzzer as output
  // PORTD &= ~(1 << BUZZER_PIN); // pin buzzer LOW
  while (1) {
    PORTD &= ~(1 << BUZZER_PIN);  // pin buzzer LOW
    static uint8_t positionInCommandTable = 0;
    static char old_key_pressed = '\0';
    char key_pressed = keypad_get_last_pressed_key();
    if (old_key_pressed != key_pressed) {
      if (key_pressed >= '0' && key_pressed <= '9') {
        if (positionInCommandTable <
            (sizeof(enteredCommandTable) / sizeof(enteredCommandTable[0]))) {
          enteredCommandTable[positionInCommandTable] = key_pressed;
          enteredCommandTable[++positionInCommandTable] = '\0';
          char print[] = {key_pressed, '\0'};
          lq_print(&device, print);
        }
      } else if (key_pressed == (char)ENTER_KEY) {
        printf("%s\n", enteredCommandTable);
        if ((enteredCommandTable[0] == '2' && enteredCommandTable[1] == '1' &&
             enteredCommandTable[2] == '3' && enteredCommandTable[3] == '7' &&
             enteredCommandTable[4] == '\0')) {
          papaj_event();
        }
        lq_clear(&device);
        uint8_t i = 0;
        for (i = 0;
             i < (sizeof(enteredCommandTable) / sizeof(enteredCommandTable[0]));
             i++) {
          enteredCommandTable[i] = ' ';
        }
        positionInCommandTable = 0;  // back to end
      } else if (key_pressed == (char)PRESET_1_KEY) {
        activateBomb(memory.mem_preset1, memory.password, &device);
      } else if (key_pressed == (char)PRESET_2_KEY) {
        activateBomb(memory.mem_preset2, memory.password, &device);
      } else if (key_pressed == (char)PRESET_3_KEY) {
        activateBomb(memory.mem_preset3, memory.password, &device);
      } else if (key_pressed == (char)SETTINGS_ENTER_KEY) {
        settings_menu(&device);
      } else if (key_pressed == (char)BATTERY_CHECK_KEY) {
        check_battery(&device);
      }
    }
    old_key_pressed = key_pressed;
  }
}
