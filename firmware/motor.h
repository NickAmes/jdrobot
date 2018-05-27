/* Junior Design Sp2018 Final Project
 * Robot Firmware - Motor Control
 * Nick Ames 2018
 */
#include <stdint.h>

/* Setup motor control. */
void motor_init(void);

/* Set left motor.
 * power ranges from -255 to 255. */
void motor_lpower(int16_t power);

/* Set right motor.
 * power ranges from -255 to 255. */
void motor_rpower(int16_t power);