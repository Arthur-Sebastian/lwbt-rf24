/*
 * LWBT-RF24
 *
 * Minimal ATmegaX8 SPI driver
 * (C) Artur Sebastian Miller 2023
*/

#include "config.h"


void spi_init(void);


uint8_t spi_transfer(uint8_t data);


inline void spi_clearSelect(void)
{
	SPI_PORT_SS |= (1 << SPI_SS);
}


inline void spi_setSelect(void)
{
	SPI_PORT_SS &= ~(1 << SPI_SS);
}


inline void spi_setLSBFirst(void)
{
	SPCR |= (1 << DORD);
}


inline void spi_setMSBFirst(void)
{
	SPCR &= ~(1 << DORD);
}
