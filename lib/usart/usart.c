#include "usart.h"

void USART_Init(unsigned int ubrr) {
  // Setting the transmission speed
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  // Turn on the transmitter and receiver
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  // Frame format setting: 8 data bits, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(unsigned char data) {
  // Wait until the transmitter buffer is ready for sending
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  // Place the data in the buffer and initiate transmission
  UDR0 = data;
}

int USART_Transmit_printf(char data, FILE *stream) {
  USART_Transmit(data);
  return 0;
}
