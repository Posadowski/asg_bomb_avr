#include "bomb_machinery.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>

#include "../lib/keypad/keypad.h"
#include "../lib/memory/memory.h"
#include "../lib/task_machinery/task_machinery.h"
#include "../lib/usart/usart.h"
#define TRUE 1
#define FALSE 0

extern task_queue *head;
extern mem_data memory;
uint8_t timerON = FALSE;
uint16_t time_to_explode = 0;

void calculate_time_to_explode(void *arg) {
  if (time_to_explode > 1) {
    taskMachinery_engque(&head, 1000, calculate_time_to_explode, NULL);
  } else {
    timerON = FALSE;
    time_to_explode = 0;
    return;
  }
  time_to_explode--;
}

void activateBomb(uint16_t timeToExplode, const char *PassToDefused,
                  LiquidCrystalDevice_t *lcd) {
  uint8_t bombMachinery_positionInCommandTable = 0;
  char bombMachinery_enteredCommandTable[LCD_LINE_LENGTH] = {};
  time_to_explode = timeToExplode;
  char old_key_pressed = '\0';
  if (strlen(PassToDefused) > (sizeof(bombMachinery_enteredCommandTable) /
                               sizeof(bombMachinery_enteredCommandTable[0]))) {
    lq_clear(lcd);
    lq_setCursor(lcd, 0, 0);
    lq_print(lcd, "password too long");
    return;
  }

  taskMachinery_engque(&head, 1000, calculate_time_to_explode, NULL);
  timerON = TRUE;

  uint8_t bombArmed = TRUE;
  uint8_t bombDefused = FALSE;

  while (bombArmed) {
    if (time_to_explode > 1) {
      lq_setCursor(lcd, 0, 0);
      lq_print(lcd, "time left: ");
      char string_time_to_print[LCD_LINE_LENGTH];
    // Calculating minutes and seconds
    unsigned int minutes = time_to_explode / 60;
    unsigned int seconds = time_to_explode % 60;
    // Format time to mm:ss
    sprintf(string_time_to_print, "%02u:%02u", minutes, seconds);
    lq_print(lcd, string_time_to_print);
    } else {
      // tone(BUZZER_PIN, NOTE_C6);
      lq_clear(lcd);
      lq_setCursor(lcd, 0, 0);
      lq_print(lcd, "BOOOOM!!!!");
      bombArmed = FALSE;
    }
    char key_pressed = keypad_get_last_pressed_key();
    if (old_key_pressed != key_pressed) {
      if (key_pressed >= '0' && key_pressed <= '9') {
        if (bombMachinery_positionInCommandTable <
            (sizeof(bombMachinery_enteredCommandTable) /
             sizeof(bombMachinery_enteredCommandTable[0]))) {
          lq_setCursor(lcd, 1,bombMachinery_positionInCommandTable);
          char print[] = {key_pressed, '\0'};
          lq_print(lcd, print);
          uint8_t i = 0;
          for (i = 0;
               i < LCD_LINE_LENGTH - bombMachinery_positionInCommandTable;
               i++) {
            lq_print(lcd, " ");
          }
          bombMachinery_enteredCommandTable
              [bombMachinery_positionInCommandTable] = (char)key_pressed;
          bombMachinery_enteredCommandTable
              [++bombMachinery_positionInCommandTable] = '\0';
        } else {
            uint8_t i;
          for (i = 0;
               i < (sizeof(bombMachinery_enteredCommandTable) /
                    sizeof(bombMachinery_enteredCommandTable[0]));
               i++) {
            bombMachinery_enteredCommandTable[i] = ' ';
          }
        }
      } else if (key_pressed == ENTER_KEY) {
        uint8_t pass_wrong = FALSE;
        uint8_t j;
        for (j = 0; j < bombMachinery_positionInCommandTable + 1;
             j++)  // +1 to catch \0
        {
          if (bombMachinery_enteredCommandTable[j] == '\0' &&
              PassToDefused[j] == bombMachinery_enteredCommandTable[j] &&
              !pass_wrong && !bombDefused) {
            bombDefused = TRUE;
            bombArmed = FALSE;
          }
          if (PassToDefused[j] != bombMachinery_enteredCommandTable[j]) {
            pass_wrong = TRUE;
          }
        }

        if (pass_wrong) {
          lq_clear(lcd);
          lq_setCursor(lcd, 1, 0);
          lq_print(lcd, "password wrong");
        }
        uint8_t k;
        for (k = 0; k < (sizeof(bombMachinery_enteredCommandTable) /
                                 sizeof(bombMachinery_enteredCommandTable[0]));
             k++) {
          bombMachinery_enteredCommandTable[k] = ' ';
        }
        bombMachinery_positionInCommandTable = 0;  // back to start
      }
    }

    old_key_pressed = key_pressed;
  }
  if (bombDefused) {
    // noTone(BUZZER_PIN);
    lq_clear(lcd);
    lq_setCursor(lcd, 0, 0);
    lq_print(lcd, "Bomb defused :D");
  }
  uint8_t exited = FALSE;
  while (!exited) {
    char key_pressed = keypad_get_last_pressed_key();
    if (old_key_pressed != key_pressed) {
        if (key_pressed == ENTER_KEY) {
          lq_clear(lcd);
          exited = TRUE;
        }
      old_key_pressed = key_pressed;
    }
  }
}
