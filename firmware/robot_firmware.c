#include <avr/interrupt.h>
#define F_CPU 10000000UL
#include <util/delay.h>

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

void motor_power(int8_t left, int8_t right){
	OCR0A = 130;
	OCR0B = 160;
	if(left > 0){
		OCR0B = left << 1;
		set(PORTC, 5);
		clr(PORTC, 4);
	} else if(left == 0){
		OCR0B = 255;
		set(PORTC, 5);
		set(PORTC, 4);
	} else if(left < 0 && left > -128){
		OCR0B = (-left) << 1;
		clr(PORTC, 5);
		set(PORTC, 4);
	} else {
		OCR0B = 255;
		clr(PORTC, 5);
		set(PORTC, 4);
	}
	if(right > 0){
		OCR0A = right << 1;
		set(PORTC, 3);
		clr(PORTC, 2);
	} else if(right == 0){
		OCR0A = 255;
		set(PORTC, 3);
		set(PORTC, 2);
	} else if(right < 0 && right > -128){
		OCR0A = (-right) << 1;
		clr(PORTC, 3);
		set(PORTC, 2);
	} else {
		OCR0A = 255;
		clr(PORTC, 3);
		set(PORTC, 2);
	}
}

void init(void){
	DDRC |= _BV(5) | _BV(4) | _BV(3) | _BV(2);
	DDRD |= _BV(5) | _BV(6);
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(CS01) | _BV(CS00); /* Set CLKDIV = 64 */
}


int main(void){
	init();
	motor_power(0, 0);
	while(1){
		/* Infinite loop. */
	}
	return(0);
}