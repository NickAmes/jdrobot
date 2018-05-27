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
void spi_tx(const uint8_t *addr, uint8_t len){
	for(int8_t i=-1; i<len; i++){
		SPDR0 = addr[(i < 0) ? 0 : i];
		wait_spif();
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
	uint8_t reg_num = SPDR0;
	uint8_t buf[3] = {0x10, 0x30, 0x70};
	spi_tx(buf, 3);
	log("SPI Done");
	
	/* Clear SPIF flag */
	reg_num = SPSR0;
	reg_num = SPDR0;
}