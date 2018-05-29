/* Junior Design Sp2018 Final Project
 * Robot Firmware - Battery Stuff
 * Nick Ames 2018
 */
#include "battery.h"
#include <avr/interrupt.h>

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))


/* Get status of battery charging. */
bool batt_charging(void){
	clr(DDRB, PB0);
	set(PORTB, PB0);
	return !(PINB & _BV(PB0));
}

/* Get battery charge percentage,
 * as an int from 0-100. */
uint8_t batt_percent(void){
	return 0;
}

/* Perform an ADC conversion and return the result. */
static uint16_t do_conversion(void){
	//start ADC conversion
	ADCSRA |= _BV(ADSC);
	
	//wait for conversion to complete
	while(ADCSRA & _BV(ADSC));

	//get ADC values from memory
	uint8_t lsb = ADCL;
	uint8_t msb = ADCH;
	uint16_t adc = msb << 8 | lsb;
	return adc;
}

/* Get battery voltage in mv. */
int16_t batt_mv(void){
	set(DIDR0, ADC0D); /* Disable input buffer on ADC0. */
	ADMUX = _BV(REFS1);
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	do_conversion();
	uint32_t adc = do_conversion();
	return (adc * 1771) / 100;
}