
/* ASG bomb  */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifdef F_CPU
#undef F_CPU
#endif

#define F_CPU 16000000UL
#define TIMER1_PRESCALER 1024
#define TIMER1_COMPARE_MATCH (F_CPU / TIMER1_PRESCALER / 1000) // 1 mili second

#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

#include "../lib/lcd/lcd.h"
#include "../lib/keypad/keypad.h"
#include "../lib/usart/usart.h"
#include "../lib/task_machinery/task_machinery.h"

task_queue *head = NULL;

void readInputs()
{
	if (!(PINB & (1 << R1_PIN)))
	{
		printf("R1\r\n");
	}
	if (!(PINB & (1 << R2_PIN)))
	{
		printf("R2\r\n");
	}
	if (!(PINB & (1 << R3_PIN)))
	{
		printf("R3\r\n");
	}
	if (!(PINB & (1 << R4_PIN)))
	{
		printf("R4\r\n");
	}
}

void queue_test(void *arg)
{
	printf("taskID %u", head->taskID);
	taskMachinery_engque(&head, 5000, queue_test, NULL);
}

FILE USART_Transmit_stream = FDEV_SETUP_STREAM(USART_Transmit_printf, NULL, _FDEV_SETUP_WRITE);

ISR(TIMER1_COMPA_vect)
{
	task_queue *current = head;
	while (current != NULL)
	{
		current->time_to_execute--;
		if (current->time_to_execute == 0)
		{
			if (current->callback != NULL)
			{
				current->callback(current->data);
			}
			// Usunięcie elementu z kolejki
			if (current->prev != NULL)
			{
				current->prev->next = current->next;
			}
			else
			{
				head = current->next;
			}
			if (current->next != NULL)
			{
				current->next->prev = current->prev;
			}
			task_queue *temp = current;
			current = current->next;
			free(temp);
		}
		else
		{
			current = current->next;
		}
	}
}

void init_timer()
{
	TCCR1B |= (1 << WGM12);				 // Tryb CTC
	TIMSK1 |= (1 << OCIE1A);			 // Zezwól na przerwanie na porównanie
	OCR1A = TIMER1_COMPARE_MATCH;		 // Ustaw czas porównania
	TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
	sei();
}

int main(void)
{
	USART_Init(MYUBRR);
	stdout = &USART_Transmit_stream;
	//_delay_ms(2000);
	DDRB = 0xff;

	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS); // intialize 4-lines display
	lq_turnOnBacklight(&device);									  // simply turning on the backlight

	lq_print(&device, "TEST!");
	lq_setCursor(&device, 1, 0); // moving cursor to the next line
	lq_print(&device, "How are you?");

	keypad_init(); // define pins in lib/keypad/keypad.h

	taskMachinery_engque(&head, 5000, queue_test, NULL);
	taskMachinery_engque(&head, _KEYPAD_CHECK_TIME, keypad_check_key_pressed, NULL);
	init_timer();

	while (1)
	{
		static uint8_t displayChanged = 1;
		static char pressed[16] = {};
		if (pressed[0] != keypad_get_last_pressed_key())
		{
			pressed[0] = keypad_get_last_pressed_key();
			displayChanged = 1;
		}
		if (displayChanged == 1)
		{
			displayChanged = 0;
			printf("__________________\r\n");
			lq_clear(&device);
			lq_setCursor(&device, 0, 0);
			lq_print(&device, "Current pressed: ");
			lq_setCursor(&device, 1, 0);

			if (pressed[0] != '\0')
			{
				printf("pressed %s\r\n", pressed);
				lq_print(&device, pressed);
			}
			else
			{
				lq_print(&device, "               ");
			}
		}
		//_delay_ms(1000);
	}
}
