/* Junior Design Sp2018 Final Project
 * Robot Firmware - Motor Control
 * Nick Ames 2018
 */
#include "motor.h"
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/atomic.h>
#include "protocol.h"
#include "uart.h"

#define set(port, bit) (port |= _BV(bit))
#define clr(port, bit) (port &= ~_BV(bit))

/* Setup motor control. */
void motor_init(void){
	DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
	DDRE |= _BV(PE2) | _BV(PE3);
	DDRC &= ~(_BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	PORTC &= ~(_BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4));
	
	set(DDRC, PC0);
 	set(PORTD, PD7); /* Sleep off. */
	clr(PORTC, PC0); /* Mode = fast decay */
	clr(PORTE, PE3); /* Mode = fast decay */
	
	PCMSK2 |= _BV(PCINT17) | _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20);
	PCICR |= _BV(PCIE2);
	
	/* Setup motor PWM timer. */
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(CS12);
	
	/* Setup 10Hz Speed Regulation Timer */
	TIMSK3 = _BV(OCIE3A);
	OCR3A = F_CPU / (10 * 64);
	TCCR3B = _BV(WGM32) | _BV(CS31) | _BV(CS30);
}

/* Set left motor.
 * power ranges from -255 to 255. */
void motor_lpower(int16_t power){
	OCR1AL = abs(power);
	if(power > 0){
		set(PORTE, PE2);
	} else {
		clr(PORTE, PE2);
	}
}

/* Set right motor.
 * power ranges from -255 to 255. */
void motor_rpower(int16_t power){
	OCR1BL = abs(power);
	if(power > 0){
		set(PORTD, PD6);
	} else {
		clr(PORTD, PD6);
	}
}

/* Encoder Counts */
static volatile int16_t CountL, CountR;

/* Encoder Interrupt
 * Increments/Decrements CountL and CountR. */
/* */
static volatile uint8_t PrevPINC;
ISR(PCINT2_vect, ISR_NAKED){
	asm volatile (
	"sbi 0x0B, 2 ; set PD2 TODO \n"
	
	"push r0        ; pop SREG\n"
	"in   r0, 0x3f  ; pop SREG\n"
	"push r0\n"
	"push XL\n"
	"push XH\n"
	
	"ldi XL, lo8(PrevPINC)\n"
	"ldi XH, hi8(PrevPINC)\n"
	
	"pop XH\n"
	"pop XL\n"
	"pop  r0       ; pop SREG\n"
	"out  0x3f, r0 ; pop SREG\n"
	"pop  r0 \n"
	
	"cbi 0x0B, 2 ; clr PD2 TODO \n"
	
	"reti                       \n"
	);
}

// ISR(PCINT2_vect){
// 	/* Direction = a_new ^ b_prev
// 	 * Count = (a_new ^ a_old) | (b_new ^ b_old)
// 	 * PINC: msb X X X RB RA LB LA X lsb */
// 	static uint8_t prev_pinc;
// 	uint8_t pinc;
// // 	set(PORTD, PD2);
// 	asm volatile ("sbi 0x0B, 2\n");
// 	pinc = PINC;
// 	if((pinc ^ prev_pinc) & 0x18){ /* Count Right */
// 		if((pinc ^ (prev_pinc >> 1)) & 0x08){
// 			CountR++;
// 		} else {
// 			CountR--;
// 		}
// 	}
// 	if((pinc ^ prev_pinc) & 0x06){ /* Count Left */
// 		if((pinc ^ (prev_pinc >> 1)) & 0x02){
// 			CountL--;
// 		} else {
// 			CountL++;
// 		}
// 	}
// 	prev_pinc = pinc;
// // 	clr(PORTD, PD2);
// 	asm volatile ("cbi 0x0B, 2\n");
// }

/* Set true every 100ms to regulate speed control. */
volatile bool DoSpeed;

/* Speed Control Interrupt */
ISR(TIMER3_COMPA_vect){
	DoSpeed = true;
}

#define COUNT_RPM_FACTOR 2.72k
#define CLAMP(var, min, max) ((var > max) ? max : ((var < min) ? min : var))
#define IABS(var) ((var < 0) ? -var : var)
static inline accum speed(volatile int16_t *countvar){
	accum speed;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		speed = *countvar;
		*countvar = 0;
	}
	return speed / COUNT_RPM_FACTOR;
}

/* Calculate PID and update the given struct. */
void calc_pid(pid_t *pid){
	accum err = pid->target - pid->current;
	accum derr = err - pid->prev_err;
	pid->i_accum += err;
	pid->i_accum = CLAMP(pid->i_accum, -pid->i_limit, pid->i_limit);
	if((err > 0 && pid->prev_err < 0) || (err < 0 && pid->prev_err > 0)){
		pid->i_accum = 0;
	}
	if(pid->target > 0 && pid->current == 0){
		pid->output = MOTOR_PRESTART;
	}
	if(pid->target == 0k){
		pid->output = 0;
		pid->i_accum = 0;
	} else {
		pid->output += pid->kp*err + pid->ki*pid->i_accum + pid->kd*derr;
		if(pid->target > 0){
			pid->output = CLAMP(pid->output, 0k, 255k);
		} else {
			pid->output = CLAMP(pid->output, -255k, 0k);
		}
	}
	pid->prev_err = err;
}

/* Setup the PID struct for a single motor. */
static void motor_pid_setup(pid_t *pid){
	pid->kp = get_motor_kp();
	pid->ki = get_motor_ki();
	pid->kd = get_motor_kd();
	pid->i_limit = MOTOR_I_LIMIT;
}

/* Print logging info about a motor PID loop. */
void pid_log(pid_t *pid){
	printf("Current RPM:\t% 3d\tTarget RPM:\t% 3d\tPower:\t%d", (int) pid->target, (int) pid->current, (int) pid->output);
	
	printf("\t 0|");
	for(int i=0;i<30;i++){
		if(pid->current > i*10){
			/* UTF-8 FULL BLOCK */
			uart_putchar(0xE2);
			uart_putchar(0x96);
			uart_putchar(0x88);
		} else {
			uart_putchar(' ');
		}
	}
	printf("| 300\n");
}
/* Do PI speed control updates. */
void motor_pid(void){
	set(PORTD, PD3);
	static pid_t lpid, rpid;
	motor_pid_setup(&lpid);
	motor_pid_setup(&rpid);
	lpid.target = get_motor_l_target();
	rpid.target = get_motor_r_target();
	lpid.current = speed(&CountL);
	rpid.current = speed(&CountR);
	set_motor_l_speed(lpid.current);
	set_motor_r_speed(rpid.current);
	calc_pid(&lpid);
	calc_pid(&rpid);
	set_motor_l_power(lpid.output);
	set_motor_r_power(rpid.output);
	motor_lpower(lpid.output);
	motor_rpower(rpid.output);
	pid_log(&rpid);
	clr(PORTD, PD3);
}