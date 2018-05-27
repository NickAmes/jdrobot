/* Junior Design Sp2018 Final Project
 * Robot Firmware - Motor Control
 * Nick Ames 2018
 */
#include "motor.h"
#include <avr/interrupt.h>

/* Setup motor control. */
void motor_init(void){
	DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
	DDRE |= _BV(PE2) | _BV(PE3);
	
}

/* Set left motor.
 * power ranges from -255 to 255. */
void motor_lpower(int16_t power){
	
}

/* Set right motor.
 * power ranges from -255 to 255. */
void motor_rpower(int16_t power){
	
}