#include "settings_menu.h"

#include <string.h>
#include <util/delay.h>

#include "../lib/keypad/keypad.h"
#include "../lib/usart/usart.h"

#define AVALIABLE_SETTINGS_OPTIONS 6

#define PASSWORD_LINE "Change pass\0"

#define PRESET1_LINE "Change set1\0"
#define PRESET2_LINE "Change set2\0"
#define PRESET3_LINE "Change set3\0"
#define VOLUME_LINE "Change volume\0"
#define RESET_LINE "Factory Reset\0"
#define RETURN_LINE "Return\0"
#define CLEAR_LINE " \0"

#define ERROR_TOO_LONG UINT16_MAX

enum {
  SELECTED_NONE = -1,
  SELECTED_CHANGE_PASS = 0,
  SELECTED_CHANGE_PRESET1,
  SELECTED_CHANGE_PRESET2,
  SELECTED_CHANGE_PRESET3,
  SELECTED_CHANGE_VOLUME,
  SELECTED_FACTORY_RESET,
  SELECTED_RETURN,
  SELECTED_MAX
};

#define TRUE 1
#define FALSE 0

extern mem_data memory;

char password[MAX_PASSWORD_LENGTH];

void (*resetFunc)(void) = 0;

int atoi(const char *str) {
  int result = 0;
  int sign = 1;
  int i = 0;

  // Skipping whitespace
  while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;

  // char handling
  if (str[i] == '-') {
    sign = -1;
    i++;
  } else if (str[i] == '+') {
    i++;
  }

  // Convert digits to integer
  while (str[i] >= '0' && str[i] <= '9') {
    result = result * 10 + (str[i] - '0');
    i++;
  }

  return sign * result;
}

void settings_menu(LiquidCrystalDevice_t *lcd) {
  uint8_t settings_positionInCommandTable = 0;
  char settings_enteredCommandTable[LCD_LINE_LENGTH] = {};
  uint8_t positon_in_settings_menu = 0;
  uint8_t screenChanged = TRUE;
  char firstLine[LCD_LINE_LENGTH] = PASSWORD_LINE;
  char secondLine[LCD_LINE_LENGTH] = PRESET1_LINE;
  char *char_asterix = "*";
  uint8_t askForFactoryReset = FALSE;
  uint8_t newValueToSet = FALSE;
  char old_key_pressed = '\n';

  while (1) {
    switch (positon_in_settings_menu) {
      case SELECTED_CHANGE_PASS:
        strcpy(firstLine, PASSWORD_LINE);
        strcpy(secondLine, PRESET1_LINE);
        strcat(firstLine, char_asterix);
        break;
      case SELECTED_CHANGE_PRESET1:
        strcpy(firstLine, PASSWORD_LINE);
        strcpy(secondLine, PRESET1_LINE);
        strcat(secondLine, char_asterix);
        break;
      case SELECTED_CHANGE_PRESET2:
        strcpy(firstLine, PRESET2_LINE);
        strcpy(secondLine, PRESET3_LINE);
        strcat(firstLine, char_asterix);
        break;
      case SELECTED_CHANGE_PRESET3:
        strcpy(firstLine, PRESET2_LINE);
        strcpy(secondLine, PRESET3_LINE);
        strcat(secondLine, char_asterix);
        break;
      case SELECTED_CHANGE_VOLUME:
        strcpy(firstLine, VOLUME_LINE);
        strcpy(secondLine, RESET_LINE);
        strcat(firstLine, char_asterix);
        break;
      case SELECTED_FACTORY_RESET:
        strcpy(firstLine, VOLUME_LINE);
        strcpy(secondLine, RESET_LINE);
        strcat(secondLine, char_asterix);
        break;
      case SELECTED_RETURN:
        strcpy(firstLine, RETURN_LINE);
        strcpy(secondLine, CLEAR_LINE);
        strcat(firstLine, char_asterix);
        break;
      default:
        printf("ERROR, positon_in_settings_menu out of range\n");
    }
    if (strlen(firstLine) >= LCD_LINE_LENGTH - 1) {
      printf("firstLine string too long\n");
      strcpy(firstLine, "ERROR");
    } else if (strlen(secondLine) >= LCD_LINE_LENGTH - 1) {
      printf("secondLine string too long\n");
      strcpy(secondLine, "ERROR");
    }
    if (screenChanged) {
      lq_setCursor(lcd, 0, 0);
      lq_print(lcd, firstLine);
      uint8_t i;
      for (i = 0; i < LCD_LINE_LENGTH;
           i++)  // TODO only clear to the end of the line
      {
        lq_print(lcd, " ");
      }
      lq_setCursor(lcd, 1, 0);
      lq_print(lcd, secondLine);
      for (i = 0; i < LCD_LINE_LENGTH; i++) {
        lq_print(lcd, " ");
      }
      screenChanged = FALSE;
    }
    char key_pressed = keypad_get_last_pressed_key();
    if (old_key_pressed != key_pressed) {
      if (key_pressed >= '0' && key_pressed <= '9') {
        if (newValueToSet == TRUE) {
          if (settings_positionInCommandTable <
              (sizeof(settings_enteredCommandTable) /
               sizeof(settings_enteredCommandTable[0])))
            lq_setCursor(lcd, 1, settings_positionInCommandTable);
          char print[] = {key_pressed, '\0'};
          lq_print(lcd, print);
          settings_enteredCommandTable[settings_positionInCommandTable] =
              (char)key_pressed;
          settings_enteredCommandTable[++settings_positionInCommandTable] =
              '\0';
        }
      } else if (key_pressed == (char)ENTER_KEY) {
        if (askForFactoryReset) {
          memory.mem_init = 0;
          write_data_to_flash(&memory);
          _delay_ms(100);
          resetFunc();  // call reset
          return;
        }
        printf("%u", positon_in_settings_menu);
        switch (positon_in_settings_menu) {
          case SELECTED_RETURN:
            printf("SELECTED_RETURN\n");
            lq_clear(lcd);
            return;
            break;
          case SELECTED_FACTORY_RESET:
            printf("SELECTED_FACTORY_RESET\n");
            lq_clear(lcd);
            lq_setCursor(lcd, 0, 0);
            lq_print(lcd, "Are u sure?");
            lq_setCursor(lcd, 1, 0);
            lq_print(lcd, "D:Yes C:Canlel");
            askForFactoryReset = TRUE;
            break;
          case SELECTED_CHANGE_PASS:
            printf("SELECTED_CHANGE_PASS\n");
            if (newValueToSet == FALSE) {
              lq_clear(lcd);
              lq_setCursor(lcd, 0, 0);
              lq_print(lcd, "Insert new pass");
              newValueToSet = TRUE;
            } else {
              if (strlen(settings_enteredCommandTable) < MAX_PASSWORD_LENGTH) {
                settings_password_set(settings_enteredCommandTable);
              } else {
                lq_clear(lcd);
                lq_setCursor(lcd, 0, 0);
                lq_print(lcd, "Pass too long\n");
                // delay(500);
              }
              newValueToSet = FALSE;
              screenChanged = TRUE;                 // need to reload screen
              settings_positionInCommandTable = 0;  // back to start
            }
            break;
          case SELECTED_CHANGE_PRESET1:
            if (newValueToSet == FALSE) {
              lq_clear(lcd);
              lq_setCursor(lcd, 0, 0);
              lq_print(lcd, "Insert set1 [s]");
              newValueToSet = TRUE;
            } else {
              if (strlen(settings_enteredCommandTable) < MAX_PASSWORD_LENGTH) {
                settings_preset1_set(atoi(settings_enteredCommandTable));
              } else {
                lq_clear(lcd);
                lq_setCursor(lcd, 0, 0);
                lq_print(lcd, "set too long");
                // delay(500);
              }
              newValueToSet = FALSE;
              screenChanged = TRUE;                 // need to reload screen
              settings_positionInCommandTable = 0;  // back to start
            }
            break;
          case SELECTED_CHANGE_PRESET2:
            if (newValueToSet == FALSE) {
              lq_clear(lcd);
              lq_setCursor(lcd, 0, 0);
              lq_print(lcd, "Insert set2 [s]");
              newValueToSet = TRUE;
            } else {
              if (strlen(settings_enteredCommandTable) < MAX_PASSWORD_LENGTH) {
                settings_preset2_set(atoi(settings_enteredCommandTable));
              } else {
                lq_clear(lcd);
                lq_setCursor(lcd, 0, 0);
                lq_print(lcd, "set too long");
                _delay_ms(500);
              }
              newValueToSet = FALSE;
              screenChanged = TRUE;                 // need to reload screen
              settings_positionInCommandTable = 0;  // back to start
            }
            break;
          case SELECTED_CHANGE_VOLUME:
            if (newValueToSet == FALSE) {
              lq_clear(lcd);
              lq_setCursor(lcd, 0, 0);
              lq_print(lcd, "Insert volume[%%]");
              newValueToSet = TRUE;
            } else {
              uint16_t instert_value = atoi(settings_enteredCommandTable);
              if(instert_value<=100 ){
                if (strlen(settings_enteredCommandTable) < 3) {
                  settings_volume_set(atoi(settings_enteredCommandTable));
                } 
              }else {
                  lq_clear(lcd);
                  lq_setCursor(lcd, 0, 0);
                  lq_print(lcd, "select from 0 to 100");
                  _delay_ms(500);
                }
              newValueToSet = FALSE;
              screenChanged = TRUE;                 // need to reload screen
              settings_positionInCommandTable = 0;  // back to start
            }
            break;
            case SELECTED_CHANGE_PRESET3:
            if (newValueToSet == FALSE) {
              lq_clear(lcd);
              lq_setCursor(lcd, 0, 0);
              lq_print(lcd, "Insert set3 [s]");
              newValueToSet = TRUE;
            } else {
              if (strlen(settings_enteredCommandTable) < MAX_PASSWORD_LENGTH) {
                settings_preset3_set(atoi(settings_enteredCommandTable));
              } else {
                lq_clear(lcd);
                lq_setCursor(lcd, 0, 0);
                lq_print(lcd, "set too long");
                _delay_ms(500);
              }
              newValueToSet = FALSE;
              screenChanged = TRUE;                 // need to reload screen
              settings_positionInCommandTable = 0;  // back to start
            }
            break;
          default:
            break;
        }
      } else if (key_pressed == (char)SETTINGS_UP_KEY) {
        if (positon_in_settings_menu <= 0) {
          positon_in_settings_menu = 0;
        } else {
          positon_in_settings_menu--;
        }
        screenChanged = TRUE;
      } else if (key_pressed == (char)SETTINGS_DOWN_KEY) {
        if (positon_in_settings_menu >= AVALIABLE_SETTINGS_OPTIONS) {
          positon_in_settings_menu = AVALIABLE_SETTINGS_OPTIONS;
        } else {
          positon_in_settings_menu++;
        }
        screenChanged = TRUE;
      } else if (key_pressed == (char)SETTINGS_CLEAR_KEY) {
        if (askForFactoryReset) {
          askForFactoryReset = FALSE;
          screenChanged = TRUE;  // need to reload screen
        } else if (newValueToSet == TRUE) {
          if (settings_positionInCommandTable <= 0) {
            settings_positionInCommandTable = 0;
            screenChanged = TRUE;  // need to reload screen
            newValueToSet = FALSE;
          } else if (settings_positionInCommandTable <
                     (sizeof(settings_enteredCommandTable) /
                      sizeof(settings_enteredCommandTable[0])))

            settings_enteredCommandTable[--settings_positionInCommandTable] =
                '\0';
          lq_setCursor(lcd, 1, settings_positionInCommandTable);
          lq_print(lcd, " ");
        }
      }
    }

    old_key_pressed = key_pressed;
  }
}



char *settings_password_set(char *password_new_value) {
  if (strlen(password_new_value) > MAX_PASSWORD_LENGTH - 1) {
    return "password too long";
  }
  strcpy(memory.password, password_new_value);
  write_data_to_flash(&memory);
  return password_new_value;
}
char *settings_password_get() {
  read_data_from_flash(&memory);
  return memory.password;
}

uint16_t settings_volume_set(uint16_t volume_new_value){
  if(volume_new_value > 100){
    return ERROR_TOO_LONG;
  }
  memory.mem_volume = volume_new_value;
  write_data_to_flash(&memory);
  return volume_new_value;
}

uint16_t settings_volume_get(){
  read_data_from_flash(&memory);
  return memory.mem_volume;
}

uint16_t settings_preset1_set(uint16_t preset1_new_value) {
  if (preset1_new_value >= UINT16_MAX - 1) {
    return ERROR_TOO_LONG;
  }
  memory.mem_preset1 = preset1_new_value;
  write_data_to_flash(&memory);
  return preset1_new_value;
}
uint16_t settings_preset1_get() {
  read_data_from_flash(&memory);
  return memory.mem_preset1;
}

uint16_t settings_preset2_set(uint16_t preset2_new_value) {
  if (preset2_new_value > UINT16_MAX - 1) {
    return ERROR_TOO_LONG;
  }

  memory.mem_preset2 = preset2_new_value;
  write_data_to_flash(&memory);
  return preset2_new_value;
}
uint16_t settings_preset2_get() {
  read_data_from_flash(&memory);
  return memory.mem_preset2;
}

uint16_t settings_preset3_set(uint16_t preset3_new_value) {
  if (preset3_new_value > UINT16_MAX) {
    return ERROR_TOO_LONG;
  }
  memory.mem_preset3 = preset3_new_value;
  write_data_to_flash(&memory);
  return preset3_new_value;
}

uint16_t settings_preset3_get() {
  read_data_from_flash(&memory);
  return memory.mem_preset3;
}
