/* Junior Design Sp2018 Final Project
 * Robot Firmware - Battery Stuff
 * Nick Ames 2018
 */
#include "config.h"
#include <stdint.h>
#include <stdbool.h>

/* Get status of battery charging. */
bool batt_charging(void);

/* Get battery voltage in mv. */
int16_t batt_mv(void);