/**
@file spi.c
@brief Minimal ATmegaX8 SPI driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include <avr/io.h>
#include <stdint.h>
#include "spi.h"


void spi_init(void)
{
	SPI_DDR |= (1 << SPI_SCK) | (1 << SPI_MOSI) | (1 << PB2);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}


uint8_t spi_transfer(uint8_t data)
{
	SPDR = data;
	while ( !(SPSR & (1 << SPIF)) );;
	return SPDR;
}
