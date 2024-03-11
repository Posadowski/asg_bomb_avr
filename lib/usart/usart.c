#include "usart.h"

void USART_Init(unsigned int ubrr) {
    // Ustawienie prędkości transmisji
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Włączenie nadajnika i odbiornika
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    // Ustawienie formatu ramki: 8 bitów danych, 1 bit stopu
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(unsigned char data) {
    // Czekaj, aż bufor nadajnika będzie gotowy do wysyłki
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Umieść dane do bufora i zainicjuj transmisję
    UDR0 = data;
}

int USART_Transmit_printf(char data, FILE *stream) {
    USART_Transmit(data);
    return 0;
}
