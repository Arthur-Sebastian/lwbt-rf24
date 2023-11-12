/*
 * LWBT-RF24
 *
 * minimal ATmegaX8 SPI driver
 * (C) Artur Sebastian Miller 2023
*/


#include <avr/io.h>
#include "spi.h"

void spi_init(void)
{
	SPI_DDR_SS |= (1 << SPI_SS);
	SPI_PORT_SS |= (1 << SPI_SS);
	SPI_DDR |= (1 << SPI_SCK) | (1 << SPI_MOSI) | (1 << SPI_SS);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}


uint8_t spi_transfer(uint8_t data)
{
	SPDR = data;
	while ( !(SPSR & (1 << SPIF)) );;
	return SPDR;
}
