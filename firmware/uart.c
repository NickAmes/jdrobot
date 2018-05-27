/* Junior Design Sp2018 Final Project
 * Robot Firmware - UART Functions
 * Nick Ames 2018
 */
#include "config.h"
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include "uart.h"

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Setup UART at BAUD 8N1. */
void uart_init(void){
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X);
	#else
	UCSR0A &= ~(1 << U2X);
	#endif
	UCSR0C = _BV(UCSZ1) | _BV(UCSZ0); /* 8-bit data */
    UCSR0B = _BV(RXEN) | _BV(TXEN);
	set(DDRD, PD1); /* UART output */
}

/* Send a character out of the UART. */
void uart_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE);
    UDR0 = c;
}

/* Get a character from the uart. */
char uart_getchar(void) {
    loop_until_bit_is_set(UCSR0A, RXC);
    return UDR0;
}

/* Send a PROGMEM string literal from the uart.
 * Usage:
 *   log("string to send");
 */
void log_helper(const char *str){
	char c;
	while((c = pgm_read_byte(str)) != '\0'){
		uart_putchar(c);
		str++;
	}
}