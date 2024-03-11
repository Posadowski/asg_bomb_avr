
/* ASG bomb  */
#include <avr/io.h>
#include <util/delay.h>

#ifdef F_CPU
#undef F_CPU
#endif

#define F_CPU 16000000UL

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include "../lib/lcd/lcd.h"
#include "../lib/keypad/keypad.h"
#include "../lib/usart/usart.h"

uint8_t rowPins[ROWS] = {R1, R2, R3,
                      R4}; // connect to the row pinouts of the keypad
uint8_t colPins[COLS] = {C1, C2, C3,
                      C4}; // connect to the column pinouts of the keypad

void readInputs(){
		if (!(PINB & (1 << R1))) {
			printf("R1\r\n");
		}  if(!(PINB & (1 << R2))){
			printf("R2\r\n");
		} if(!(PINB & (1 << R3))){
			printf("R3\r\n");
		} if(!(PINB & (1 << R4))){
			printf("R4\r\n");
		}
}					  
FILE USART_Transmit_stream = FDEV_SETUP_STREAM(USART_Transmit_printf, NULL, _FDEV_SETUP_WRITE);
int main(void)
{	
	USART_Init(MYUBRR);
	stdout = &USART_Transmit_stream;
	//_delay_ms(2000);
	DDRB=0xff;
	
	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS); // intialize 4-lines display
	lq_turnOnBacklight(&device); // simply turning on the backlight

	lq_print(&device, "Hello world!");
	lq_setCursor(&device, 1, 0); // moving cursor to the next line
	lq_print(&device, "How are you?");

	keypad_init();

	printf("Hello\r\n");

	while(1)
	{
    printf("__________________\r\n");
	// Ustawienie stanu niskiego na wyjściu
    PORTB &= ~(1 << C1);
	readInputs();
	DDRB |= (1 << C1);


	
	_delay_ms(1000);

	}
}