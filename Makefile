# Makefile for AVR project

# Compiler and flags
CC = avr-gcc
OBJCOPY = avr-objcopy
MCU = atmega328p
F_CPU = 16000000UL
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
PORT=COM4

# Directories
SRC_DIR = src
LIB_DIR = lib
LCD_DIR = $(LIB_DIR)/lcd
KEYPAD_DIR = $(LIB_DIR)/keypad
USART_DIR = $(LIB_DIR)/usart
BUILD_DIR = build

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(LIB_DIR)/**/*.c) 
OBJ_FILES = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRC_FILES)))

# Target
TARGET = $(BUILD_DIR)/main

all: create_build_dir $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).elf: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/**/%.c
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@	

create_build_dir:
	@mkdir -p $(BUILD_DIR)

install: ${TARGET}.hex
	avrdude -v -c arduino -p ${MCU} -P ${PORT} -b 115200 -U flash:w:$<

clean:
	rm -rf $(BUILD_DIR)
