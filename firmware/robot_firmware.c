/* Junior Design Sp2018 Final Project
 * Robot Firmware
 * Nick Ames 2018
 */
#include <avr/interrupt.h>
#define F_CPU 8000000UL //TODO: Change when oscillator fixed. 
#include <util/delay.h>
#define BAUD 9600
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Setup UART at BAUD 8N1. */
void init_uart(void){
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X);
	#else
	UCSR0A &= ~(1 << U2X);
	#endif
	UCSR0C = _BV(UCSZ1) | _BV(UCSZ0); /* 8-bit data */
    UCSR0B = _BV(RXEN) | _BV(TXEN); 
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

/* Send a PROGMEM string out of the UART. */

/* Send a PROGMEM string literal from the uart. */
#define log(str) fsfsadffasf

/* Initialize system. */
void init(void){
	set(DDRD, PD1); /* UART output */
	init_uart();
}

int main(void){
	init();
	uart_putchar('h');
	while(1){
		uart_putchar(uart_getchar());
	}
	return(0);
}