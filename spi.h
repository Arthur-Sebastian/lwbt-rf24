/*
 * LWBT-RF24
 *
 * Minimal ATmegaX8 SPI driver
 * (C) Artur Sebastian Miller 2023
*/


void spi_init(void);


void spi_toggleSelect(void);


void spi_swapDataOrder(void);


uint8_t spi_transfer(uint8_t data);
