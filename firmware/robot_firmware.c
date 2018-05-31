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
#include "battery.h"
#include "led.h"

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
	set(DDRD, PD2);
	set(DDRD, PD3);
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
 		if(DoSpeed){
			DoSpeed = false;
// 			motor_rpower(get_motor_r_power());
// 			motor_lpower(get_motor_l_power());
			motor_pid();
			set_batt_mv(batt_mv());
			led(get_led_r(), get_led_g(), get_led_b());
		}
	}
	return(0);
}