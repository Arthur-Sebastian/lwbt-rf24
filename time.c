/**
@file time.c
@brief Minimal ATmegaX8 timer driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "config.h"
#include "time.h"
#include "uart.h"


static uint32_t ov_ctr = 0;


ISR(TIMER0_OVF_vect, ISR_NOBLOCK)
{
	/* there is no way that this interrupt fires twice,
	 * skip disabling its source for speed */
	ov_ctr++;
}


void tm_init(void)
{
	ov_ctr = 0;
	/* enable TIMER0 interrupt */
	TIMSK0 |= (1 << TOIE0);
	/* start TIMER0 @ 2MHz */
	TCCR0B = (TCCR0B&0b11111000) | (1 << CS01);
}


uint32_t tm_ms(void)
{
	TIMSK0 &=~ (1 << TOIE0);
	uint32_t temp = ov_ctr >> 3;
	TIMSK0 |= (1 << TOIE0);
	return temp;
}


void tm_halt(uint16_t ms)
{
	uint32_t temp = ov_ctr;
	ov_ctr = 0;
	while(tm_ms() < ms);;
	ov_ctr = temp;
}


void tm_reset(void)
{
	ov_ctr = 0;
}
