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
}

static inline void wait_spif(void){
	while(!(SPSR0 & _BV(SPIF)) && !CS_STATE()){
		/* Wait until transmission or transaction ends. */
	}
}

/* Send a memory block over spi. */
void spi_tx(uint8_t *addr, uint8_t len){
	for(int8_t i=0; i<len; i++){
		wait_spif();
		SPDR0 = addr[i];
	}
	wait_spif();
}

/* Receive a memory block over spi. */
void spi_rx(uint8_t *addr, uint16_t len){
	wait_spif();
	uint8_t dummy = SPDR0;
	dummy = dummy;
	for(uint8_t i=0; i<len; i++){
		wait_spif();
		addr[i] = SPDR0;
	}
}
