/* Junior Design Sp2018 Final Project
 * Robot Firmware
 * Nick Ames 2018
 */
#include "config.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>
#include "uart.h"
#include "spi.h"

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

#define speaker_on()  set(PORTB, PB2)
#define speaker_off() clr(PORTB, PB2)

/* Initialize system. */
void init(void){
	uart_init();
	spi_init();
	set(DDRB, PB2);
	speaker_on();
	sei();
	log("Junior Design Robot Firmware Built ");
	log(__DATE__);
	log(" ");
	log(__TIME__);
	log("\n");
	log("Startup.\n");
}

int main(void){
	init();
	while(1){
		log("Hello world!\n");
		_delay_ms(1000);
	}
	return(0);
}