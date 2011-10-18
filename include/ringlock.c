/*
 *  ringlock.c
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


#include "ringlock.h"


void rl_init(void) {

	//Setting up pin to pass on the token
	RL_OUT_DDR |= (1 << RL_OUT);
	RL_OUT_PORT &= ~(1 << RL_OUT);

	//Setting up the interrupt use to receive the token
	RL_INT_DDR &= ~(1 << RL_INT_PIN);
	RL_INT_CONFIG |= RL_INT_CBITS;
	RL_INT_ENABLE |= (1 << RL_INT_EBITS);

	rl_request_token = 0;

#ifdef RL_MASTER
	rl_have_token = 1; 	//Master haz the token

	rl_to_setup();		//Fire up the timer
	rl_to_start();
#else
	rl_have_token = 0;
#endif
}

/*
 *	Interrupt used for getting the token and timeout
 */
ISR(RL_INT) {
	rl_have_token = 1;
	if(!rl_request_token) {
		rl_release_lock();
	}
	else {
		rl_request_token = 0;
	}
#ifdef RL_MASTER
	rl_to_reset();	// reset the timeout
#endif

}

#ifdef RL_MASTER
ISR(RL_TO_INT, ISR_ALIASOF(RL_INT)); 	// if we have a timeout,
										// we just pretend to have the token
#endif
