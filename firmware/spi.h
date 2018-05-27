/* Junior Design Sp2018 Final Project
 * Robot Firmware - SPI Support
 * Nick Ames 2018
 */

/* Initialize the SPI bus as a slave. */
void spi_init(void);

/* Send a memory block over spi. */
void spi_tx(const uint8_t *addr, uint16_t len);

/* Receive a memory block over spi. */
void spi_rx(uint8_t *addr, uint16_t len);