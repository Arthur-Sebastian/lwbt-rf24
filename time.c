/**
@file time.c
@brief Minimal ATmegaX8 timer driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "time.h"

uint32_t ovf = 0;
uint8_t tm_half = 0;


void tm_init(void)
{
	ovf = 0;
	/* enable TIMER0 interrupt */
	TIMSK0 |= (1<<TOIE0);
	/* start TIMER0 @ 2MHz */
	TCCR0B = (TCCR0B&0b11111000) | (1<<CS01);
}


ISR(TIMER0_OVF_vect)
{
	ovf++;
}


uint32_t tm_ms(void)
{
	cli();
	uint32_t temp = ovf >> 3;
	sei();
	return temp;
}


void tm_halt(uint16_t ms)
{
	uint32_t temp = ovf;
	ovf = 0;
	while(tm_ms() < ms);;
	ovf = temp;
}


void tm_reset(void)
{
	ovf = 0;
}

