/* Junior Design Sp2018 Final Project
 * Robot Firmware - LED Control
 * Nick Ames 2018
 */
#include "led.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Set the RGB LED color brightnesses.
 * Each ranges from 0-255. */
void led(uint8_t r, uint8_t g, uint8_t b){
	set(DDRA, PA7);
	clr(PORTA, PA7);
	_delay_us(70);
	
	asm volatile ("\
	nop \n\
	nop           \n\
	nop           \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 7    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 6    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 5    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 4    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 3    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 2    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 1    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %0, 0    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 7    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 6    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 5    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 4    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 3    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 2    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 1    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %1, 0    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 7    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 6    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 5    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 4    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 3    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 2    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 1    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	sbi 0x02, 7   \n\
	sbrs %2, 0    \n\
	cbi 0x02, 7   \n\
	nop           \n\
	nop           \n\
	nop           \n\
	cbi 0x02, 7   \n\
	nop           \n\
\
	" :  :
	"r" (g), "r" (r), "r" (b));
	
	r=r;
	g=g;
	b=b;
}