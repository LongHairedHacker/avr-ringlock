/*
 *  ringlock.h
 *
 *  Created on: Sep 28, 2011
 *  Author: sebastian
 *
 *	This file is part of the RingLock-library for Atmel AVR MCUS.
 *
 * 	RingLock is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	RingLock is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with RingLock.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	Copyright Sebastian Schumb (sebastian_at_sebastians-site_de) 2011
 */


#ifndef RINGLOCK_H_
#define RINGLOCK_H_ RINGLOCK_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifdef __AVR_ATmega32__

/*
 * 	Config for the Port that passes the token on
 */
#define RL_OUT_PORT 	PORTD		// Port used for passing the token,
#define RL_OUT_DDR		DDRD		// DDRD for that port
#define RL_OUT 			PD5			// and the pin we want to use

/*
 * 	Config for the external interrupt
 */
#define RL_INT_DDR		DDRD		// DDR
#define RL_INT_PIN		PIN2		// and the Pin used for the interrupt
#define RL_INT			INT0_vect	// The timer interrupt
#define RL_INT_CONFIG	MCUCR		// Interrupt config register
#define RL_INT_CBITS	((1 << ISC00) | (1 << ISC01))	// bits to set in this register
#define RL_INT_ENABLE	GICR		// Global Interrupt Register
#define RL_INT_EBITS	INT0		// Bit that enables the interrupt

/*
 * 	Timer config
 * 	The master has a timeout timer, just in case a token gets lost,
 * 	or you have a controller with messed up code in the ring ...
 */
#define RL_TO_TIME				10  				//timeout in milliseconds
#define RL_TO_INT				TIMER0_COMP_vect	//timer interrupt
/*
 * You can't use the config register and bits approach from above for timers,
 * e.g. some mcus split TCCR in two registers.
 * Therefore I cheated a little and used static inline functions,
 * to make the timers configurable.
 */

// function called to setup the timer
static inline void rl_to_setup(void) {
	TCCR0 |= (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0 = ((RL_TO_TIME * F_CPU/1000) / 1024); 	//1024 is prescaler
}

// function called to start the timer
static inline void rl_to_start(void) {
	TCCR0 |= (1<<CS00) | (1<<CS02);
}

// function called to reset the timer
static inline void rl_to_reset(void) {
	TCCR0 &= ~((1<<CS00) | (1<<CS02));
	TCNT0 = 0;
	rl_to_start();
}


#elif defined __AVR_ATmega8__

/*
 * 	Config for the Port that passes the token on
 *	Explanation see above ...
 */
#define RL_OUT_PORT 	PORTD
#define RL_OUT_DDR		DDRD
#define RL_OUT 			PD0

/*
 * 	Config for the external interrupt
 * 	Explanation see above ...
 */
#define RL_INT_DDR		DDRD
#define RL_INT_PIN		PD2
#define RL_INT			INT0_vect
#define RL_INT_CONFIG	MCUCR
#define RL_INT_CBITS	((1 << ISC00) | (1 << ISC01))
#define RL_INT_ENABLE	GICR
#define RL_INT_EBITS	INT0

/*
 *  TODO: Add timer config here ;)
 */


#else
#error "MCU is not supported. Please add it to ringlock.h"
#endif


/*
 *	Config ends here
 */

volatile uint8_t rl_request_token;		// set if the controller should keep the token
volatile uint8_t rl_have_token;			// set if the controller has the token

/*
 * 	Functions for manipulation the token
 * 	The next two functions might seem necessary,
 * 	but we don't want people fiddling with the flags directly.
 */
static inline void rl_request_lock(void) {
	rl_request_token = 1;
}

static inline uint8_t rl_have_lock(void) {
	return rl_have_token;
}


static inline void rl_release_lock(void) {	// used to release the token
	if(rl_have_token) {						// make sure we have the token
		rl_have_token = 0;
		RL_OUT_PORT |= (1 << RL_OUT);
		_delay_us(1);						// give the signal time to raise
		RL_OUT_PORT &= ~(1 << RL_OUT);
	}
}

void rl_init(void);


#endif /* RINGLOCK_H_ */
