/**
@file spi.h
@brief Minimal ATmegaX8 SPI driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include "config.h"
#include <stdint.h>


#define SPI_DDR       DDRB
#define SPI_PORT      PORTB
#define SPI_SCK       5
#define SPI_MISO      4
#define SPI_MOSI      3
#define SPI_DDR_SS    DDRC
#define SPI_PORT_SS   PORTC


inline void spi_bind(uint8_t pin)
{
	SPI_DDR_SS |= (1 << pin);
	SPI_PORT_SS |= (1 << pin);
};


void spi_init(void);


uint8_t spi_transfer(uint8_t data);


inline void spi_unselect(uint8_t pin)
{
	SPI_PORT_SS |= (1 << pin);
}


inline void spi_select(uint8_t pin)
{
	SPI_PORT_SS &= ~(1 << pin);
}


inline void spi_setLSBFirst(void)
{
	SPCR |= (1 << DORD);
}


inline void spi_setMSBFirst(void)
{
	SPCR &= ~(1 << DORD);
}
