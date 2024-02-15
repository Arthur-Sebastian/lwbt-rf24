/**
@file uart.c
@brief Minimal ATmegaX8 UART driver and utilities.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include <avr/io.h>
#include <stdint.h>
#include "uart.h"

#define UBRR (F_CPU/16/BAUD-1)


static const char toHex[] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};


void uart_init(void)
{
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t) UBRR;
	UCSR0A = (1 << U2X0);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}


void uart_char(char data)
{
	while ( !(UCSR0A & (1 << UDRE0)) );;
	UDR0 = data;
}


void uart_print(char *data)
{
	while (*data != '\0') {
		while ( !(UCSR0A & (1 << UDRE0)) );;
		UDR0 = *data;
		data++;
	}
}


void uart_hex(uint8_t data)
{
	uint8_t h_hex, l_hex;
	l_hex = data & 0x0F;
	h_hex = data >> 4;
	char buf[3];
	buf[0] = toHex[h_hex];
	buf[1] = toHex[l_hex];
	buf[2] = '\0';
	uart_print(buf);
}


void uart_bin(uint8_t data)
{
	char buf[10];
	buf[9] = '\0';
	buf[8] = '\n';

	for (uint8_t i = 0; i < 8; i++) {
		buf[i] = ((data & 0x80) != 0) ? '1' : '0';
		data <<= 1;
	}

	uart_print(buf);
}


void uart_print_hex(uint8_t* data, uint8_t length)
{
	for(uint8_t i = 0; i < length; i++) {
		uart_hex(*(data + i));
	}
}
