
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
#include "bomb_machinery.h"

#define SOFTWARE_VERSION "0.4"  // 0.3 last arduino version

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

void papaj_event() {
  // TODO
  printf("JP2GMD\n");
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

  keypad_init();  // define pins in lib/keypad/keypad.h
  taskMachinery_engque(&head, 5000, queue_test, NULL);
  taskMachinery_engque(&head, _KEYPAD_CHECK_TIME, keypad_check_key_pressed,
                       NULL);
  init_timer();

  read_data_from_flash(&memory);
  if (memory.mem_init == 0) {
    printf("MEMORY INIT\r\n");
    memory.mem_preset1 = 10;
    memory.mem_preset2 = 15;
    memory.mem_preset3 = 20;
    strcpy(memory.password, "1234");

    memory.mem_init = 1;
    write_data_to_flash(&memory);
  } else {
    printf("Memory read\r\n");
    printf("preset1 %u\r\n", memory.mem_preset1);
    printf("preset2 %u\r\n", memory.mem_preset2);
    printf("preset3 %u\r\n", memory.mem_preset3);
    printf("password %s\r\n", memory.password);
  }
  lq_clear(&device);
  lq_setCursor(&device, 0, 0);
  while (1) {
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
      }else if(key_pressed == (char)PRESET_1_KEY){
        activateBomb(memory.mem_preset1,memory.password,&device);
      }else if(key_pressed == (char)PRESET_2_KEY){
        activateBomb(memory.mem_preset2,memory.password,&device);
      }else if(key_pressed == (char)PRESET_3_KEY){
        activateBomb(memory.mem_preset3,memory.password,&device);
      }
    }
    old_key_pressed = key_pressed;
    // static uint8_t displayChanged = 1;
    // static char pressed[16] = {};
    // if (pressed[0] != keypad_get_last_pressed_key()) {
    //   pressed[0] = keypad_get_last_pressed_key();
    //   displayChanged = 1;
    // }
    // if (displayChanged == 1) {
    //   displayChanged = 0;
    //   printf("__________________\r\n");
    //   lq_clear(&device);
    //   lq_setCursor(&device, 0, 0);
    //   lq_print(&device, "Current pressed: ");
    //   lq_setCursor(&device, 1, 0);

    //   if (pressed[0] != '\0') {
    //     printf("pressed %s\r\n", pressed);
    //     lq_print(&device, pressed);
    //   } else {
    //     lq_print(&device, "               ");
    //   }
    // }
  }
}
