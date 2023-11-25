/**
@file spi.h
@brief Minimal ATmegaX8 SPI driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include "config.h"
#include <stdint.h>


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
