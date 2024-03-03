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


static uint32_t ms_ctr = 0;


ISR(TIMER1_COMPA_vect, ISR_NOBLOCK)
{
	/* there is no way that this interrupt fires twice,
	 * skip disabling its source for speed */
	ms_ctr++;
}


void tm_init(void)
{
	/* start TIMER1 with @250kHz, CTC mode */
	TCCR1B = (1 << CS11) | (1 << CS10) | (1 << WGM12);
	/* 250 cycles at 250kHz = 1ms */
	OCR1AL = 250;
	/* enable TIMER1 OCM interrupt */
	TIMSK1 = (1 << OCIE1A);
}


uint32_t tm_ms(void)
{
	TIMSK1 &=~ (1 << OCIE1A);
	uint32_t temp = ms_ctr;
	TIMSK1 |= (1 << OCIE1A);
	return temp;
}


void tm_halt(uint16_t ms)
{
	uint32_t temp = ms_ctr;
	ms_ctr = 0;
	while(tm_ms() < ms);;
	ms_ctr = temp;
}


void tm_reset(void)
{
	ms_ctr = 0;
}
