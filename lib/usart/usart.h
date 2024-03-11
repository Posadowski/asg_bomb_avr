#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
int USART_Transmit_printf(char data, FILE *stream);

#endif //USART_H_