/**
@file uart.h
@brief Minimal ATmegaX8 UART driver and utilities.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef UART
#define UART

#include <stdint.h>
#include "config.h"

/* Effectively doubled by setting U2X0 bit in UCSR0A */
#define BAUD 76800


void uart_init(void);


void uart_char(char data);


void uart_print(char *data);


void uart_hex(uint8_t data);


void uart_bin(uint8_t data);


void uart_print_hex(uint8_t* data, uint8_t length);

#endif
