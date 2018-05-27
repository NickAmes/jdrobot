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
#include "motor.h"
#include "protocol.h"

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

#define speaker_on()  set(PORTB, PB2)
#define speaker_off() clr(PORTB, PB2)

/* Initialize system. */
void init(void){
	uart_init();
	spi_init();
	motor_init();
	set(DDRB, PB2);
	speaker_on();
	set(DDRD, PB2);
	set(DDRD, PB3);
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
 		motor_lpower(Data->left_power);
		motor_rpower(Data->right_power);
		_delay_ms(2);
	}
	return(0);
}