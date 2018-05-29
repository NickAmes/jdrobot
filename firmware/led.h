/* Junior Design Sp2018 Final Project
 * Robot Firmware - LED Control
 * Nick Ames 2018
 */
#include "config.h"
#include <stdint.h>
#include <stdbool.h>

/* Set the RGB LED color brightnesses.
 * Each ranges from 0-255. */
void led(uint8_t r, uint8_t g, uint8_t b);