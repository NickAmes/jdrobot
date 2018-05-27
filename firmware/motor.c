/* Junior Design Sp2018 Final Project
 * Robot Firmware - Motor Control
 * Nick Ames 2018
 */
#include "config.h"
#include "motor.h"
#include <avr/interrupt.h>
#include <stdlib.h>

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Setup motor control. */
void motor_init(void){
	DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
	DDRE |= _BV(PE2) | _BV(PE3);
	DDRC &= ~(_BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	PORTC &= ~(_BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	
	set(DDRC, PC0);
 	set(PORTD, PD7); /* Sleep off. */
	clr(PORTC, PC0); /* Mode = fast decay */
	clr(PORTE, PE3); /* Mode = fast decay */
	
	PCMSK2 |= _BV(PCINT17) | _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20);
	PCICR |= _BV(PCIE2);
	
	/* Setup motor PWM timer. */
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(CS11) | _BV(CS10);
	
	/* Setup 100Hz Speed Regulation Timer */
	TIMSK3 = _BV(OCIE3A);
	OCR3A = F_CPU / (100 * 64);
	TCCR3B = _BV(WGM32) | _BV(CS31) | _BV(CS30);
}

/* Set left motor.
 * power ranges from -255 to 255. */
void motor_lpower(int16_t power){
	OCR1AL = abs(power);
	if(power > 0){
		set(PORTE, PE2);
	} else {
		clr(PORTE, PE2);
	}
}

/* Set right motor.
 * power ranges from -255 to 255. */
void motor_rpower(int16_t power){
	OCR1BL = abs(power);
	if(power > 0){
		set(PORTD, PD6);
	} else {
		clr(PORTD, PD6);
	}
}

/* Encoder Interrupt */
ISR(PCINT2_vect){
	set(PORTD, PD2);
	clr(PORTD, PD2);
}

/* Speed Control Interrupt */
ISR(TIMER3_COMPA_vect){
	set(PORTD, PD3);
	clr(PORTD, PD3);
}