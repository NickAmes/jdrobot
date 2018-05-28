/* Junior Design Sp2018 Final Project
 * Robot Firmware - Motor Control
 * Nick Ames 2018
 */
#include "config.h"
#include "motor.h"
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/atomic.h>
#include "protocol.h"

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
	TCCR1B = _BV(CS11) | _BV(CS10);
	
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
// ISR(PCINT2_vect, ISR_NAKED){
// 	/* Direction = a_new ^ b_prev
// 	 * Count = (a_new ^ a_old) | (b_new ^ b_old) */
// 	static uint8_t prev_pinc;
// 	asm volatile ("\
// 	sbi 0x0b, 2 ; set PD2 TODO                \
// 	                                          \
// 	pop r4                                    \
// 	cbi 0x0b, 2 ; clr PD2 TODO                \
// 	");
// 	
// 	uint8_t cur_pinc = PINC;
// 	set(PORTD, PD3);
// }
ISR(PCINT2_vect){
	/* Direction = a_new ^ b_prev
	 * Count = (a_new ^ a_old) | (b_new ^ b_old)
	 * PINC: msb X X X RB RA LB LA X lsb */
	static uint8_t prev_pinc;
	uint8_t pinc;
	set(PORTD, PD2);
	pinc = PINC;
	if((pinc ^ prev_pinc) & 0x18){ /* Count Right */
		if((pinc ^ (prev_pinc >> 1)) & 0x08){
			CountR++;
		} else {
			CountR--;
		}
	}
	if((pinc ^ prev_pinc) & 0x06){ /* Count Left */
		if((pinc ^ (prev_pinc >> 1)) & 0x02){
			CountL++;
		} else {
			CountL--;
		}
	}
	prev_pinc = pinc;
	clr(PORTD, PD2);
}

/* Set true every 100ms to regulate speed control. */
volatile bool DoSpeed;

/* Speed Control Interrupt */
ISR(TIMER3_COMPA_vect){
	DoSpeed = true;
}

#define CLAMP_BYTE(var) do {var = (var > 255) ? 255 : ((var < -255) ? -255 : var); } while(0);

static inline void __attribute__((always_inline)) pi(accum kp,
                                                     accum ki,
                                                     volatile const accum *target_rpm_data,
                                                     volatile int16_t *count_isr,
                                                     volatile accum *speedout_data,
                                                     volatile int16_t *power_data,
                                                     void (*motor_setfunc)(int16_t),
                                                     accum *i_store){
	accum err, speed, target_rpm, count;
	int16_t power;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		count = *count_isr;
		*count_isr = 0;
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		target_rpm = *target_rpm_data;
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		power = *power_data;
	}
	speed = count / 2.72k;
	err = target_rpm - speed;
	(*i_store) += err;
	power += kp*err + ki*(*i_store);
	CLAMP_BYTE(power)
	if(target_rpm == 0k){
		power = 0;
		*i_store = 0;
	}
	motor_setfunc(power);
	*power_data = power;
	*speedout_data = speed;
}

/* Do PI speed control updates. */
void motor_pi(void){
	set(PORTD, PD3);
	static accum i_l, i_r;
	accum kp, ki;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		kp = Data->motor_p;
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		ki = Data->motor_i;
	}
	pi(kp, ki, &(Data->left_motor), &CountL, &(Data->left_speed),  &(Data->left_power), motor_lpower, &i_l);
	pi(kp, ki, &(Data->right_motor), &CountR, &(Data->right_speed),  &(Data->right_power), motor_rpower, &i_r);
	clr(PORTD, PD3);
}