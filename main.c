/*
 * main.c
 *
 *  Created on: Sep 28, 2011
 *      Author: sebastian
 *
 *	Makefile
 *	Created on: Sep 28, 2011
 *	Author: sebastian
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

#include <avr/io.h>
#include <util/delay.h>

#include "include/ringlock.h"

#define BAUD 9600UL		// baudrate

// Some calculations ...
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // rounding magic
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Real baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)		// Error in 0,1%

#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))		// Make sure your UBRR_VAL will work
  #error Baudrate error is bigger then 1% !
#endif



int main(void) {

	rl_init();

	UBRRH = UBRR_VAL >> 8;		//Setting baudrate
	UBRRL = UBRR_VAL & 0xFF;

	UCSRB |= (1<<TXEN);  // UART TX
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // Asynchronous 8N1


	sei();

	while(1) {

		rl_request_lock();				// Request a log
		while(!rl_have_lock());			// Wait for it
		while (!(UCSRA & (1<<UDRE))); 	// Make sure the uart is ready to go
#ifdef RL_MASTER
		UDR = 'A';						// Master send A
#else
		UDR = 'B';						// Slave send B
#endif
		while (!(UCSRA & (1<< TXC))); 	// waiting for the uart to finish transmission
		UCSRA |= (1 << TXC); 			// resetting the TXC bit
		_delay_us(250);					// We need to make sure, that no other mcu
										// is sending during our stop bit.
		rl_release_lock();				// Stop bit is over, the other can talk.
	}

	return 0;
}
