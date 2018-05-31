/* Junior Design Sp2018 Final Project
 * Robot Firmware - Motor Control
 * Nick Ames 2018
 */
#include <stdint.h>
#include <stdfix.h>
#include <stdbool.h>

/* Setup motor control. */
void motor_init(void);

/* Set true every 100ms to regulate speed control. */
extern volatile bool DoSpeed;

/* Do PI speed control updates. */
void motor_pi(void);

/* Set left motor.
 * power ranges from -255 to 255. */
void motor_lpower(int16_t power);

/* Set right motor.
 * power ranges from -255 to 255. */
void motor_rpower(int16_t power);

/* PID controller struct */
typedef struct {
	accum kp, ki, kd;
	accum target;
	accum current;
	accum output;
	accum i_accum;
	accum i_limit;
	accum prev_err;
} pid_t;

/* Calculate PID and update the given struct. */
void calc_pid(pid_t *pid);

/* Do PI speed control updates. */
void motor_pid(void);