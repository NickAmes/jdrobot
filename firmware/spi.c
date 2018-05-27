/* Junior Design Sp2018 Final Project
 * Robot Firmware - SPI Support
 * Nick Ames 2018
 */
#include "config.h"
#include "uart.h"
#include <avr/interrupt.h>

#define CS_STATE() (!!(PINB & _BV(PB4)))

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Initialize the SPI bus as a slave. */
void spi_init(void){
	set(PORTB, PB4); /* CS pull-up */
	set(DDRB, PB6); /* MISO output. */
	SPCR0 = _BV(SPIE) | _BV(SPE);
	SPDR0 = 0xAA;
}

static inline void wait_spif(void){
	while(!(SPSR0 & _BV(SPIF)) && !CS_STATE()){
		/* Wait until transmission or transaction ends. */
		set(PORTD, PB3);
	}
	clr(PORTD, PB3);
}

/* Send a memory block over spi. */
void spi_tx(uint8_t *addr, uint8_t len){
	uint8_t dummy;
	for(int8_t i=0; i<len; i++){
		wait_spif();
		SPDR0 = addr[i];
	}
	wait_spif();
}

/* Receive a memory block over spi. */
void spi_rx(uint8_t *addr, uint16_t len){
	for(uint8_t i=0; i<len; i++){
		wait_spif();
		addr[i] = SPDR0;
	}
}

ISR(SPI0_STC_vect){
	set(PORTD, PB2); //TODO
	
	uint8_t reg_num = SPDR0;
	uint8_t buf[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	spi_tx(buf, 8);
	
	/* Clear SPIF flag */
	reg_num = SPSR0;
	reg_num = SPDR0;
	clr(PORTD, PB2); //TODO
}