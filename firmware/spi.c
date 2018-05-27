/* Junior Design Sp2018 Final Project
 * Robot Firmware - SPI Support
 * Nick Ames 2018
 */
#include "config.h"
#include <avr/interrupt.h>

#define CS_STATE() (!!(PINB & _BV(PB4)))

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Initialize the SPI bus as a slave. */
void spi_init(void){
	set(PORTB, PB4); /* CS pull-up */
	set(DDRB, PB6); /* MISO output. */
}

/* Send a memory block over spi. */
void spi_tx(const uint8_t *addr, uint16_t len);

/* Receive a memory block over spi. */
void spi_rx(uint8_t *addr, uint16_t len);